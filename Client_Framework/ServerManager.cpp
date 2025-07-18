#include "stdafx.h"
#include "ServerManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "FollowTarget.h"
#include "Transform.h"
#include"Camera.h"
#include"PlayerController.h"
#include"ThirdPersonCamera.h"
#include "DX12Manager.h"
#include "ResourceManager.h"
#include"ItemMovement.h"
#include "ObjectState.h"
#include "EnemyState.h"
#include "CutScene.h"

void ServerManager::Initialize()
{
	// ------- ���� ���̱� -------------------
	std::wcout.imbue(std::locale("korean"));

	WSADATA WSAData{};
	int err = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (0 != err) {
		print_error("WSAStartup", WSAGetLastError());
	}

	server_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (server_socket == INVALID_SOCKET) {
		std::cerr << "WSASocket failed: " << WSAGetLastError() << "\n";
		WSACleanup();
		exit(1);
	}

	InitPlayerAndCamera();
}

void ServerManager::Destroy()
{
	if (server_socket != INVALID_SOCKET) {
		closesocket(server_socket);
		server_socket = INVALID_SOCKET;
	}
	WSACleanup();
	mIsConnected = false;
	mIsLoggedIn = false;
	clientID = -1;
	RenderOK = 0;
	mPlayer.reset();
	mMainCamera.reset();
	mOtherPlayers.clear();
	mItems.clear();
	mProjectiles.clear();
	mEnemies.clear();
}

void ServerManager::Connect()
{
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);

	//cout << "������ ������ �ּҸ� �Է��ϼ��� : ";
	//cin >> SERVER_ADDR;

	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

	std::cout << "Connecting to " << reinterpret_cast<sockaddr*>(&server_addr) << ":" << PORT_NUM << "\n";
	int err = connect(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (0 != err) {
		print_error("connect", WSAGetLastError());
	}
	mIsConnected = true;

	WSAOVERLAPPED wsaover;
	ZeroMemory(&wsaover, sizeof(wsaover));
}

void ServerManager::Client_Login()
{
	if (!mIsConnected) {
		std::cerr << "Not connected to server.\n";
		return;
	}
	if(mIsLoggedIn) {
		std::cerr << "Already logged in.\n";
		return;
	}
	CS_LOGIN_PACKET p;
	p.size = sizeof(p);
	p.type = CS_LOGIN;

	Send_Packet(&p);

	Recv_Loop();
	//std::thread recv_thread{ [this]() { Recv_Loop(); } };
	//recv_thread.detach();
}

bool ServerManager::InitPlayerAndCamera()
{
	mPlayer = std::make_shared<CGameObject>();
	mPlayer->SetTag("Player");
	mPlayer->SetName("Player");
	mPlayer->SetRenderLayer(RENDER_LAYER::Opaque);
	mPlayer->SetStatic(false);

	auto cutScene = mPlayer->AddComponent<CCutScene>();
	mPlayer->SetCutScene(cutScene);

	auto playerController = mPlayer->AddComponent<CPlayerController>();
	mPlayer->SetPlayerController(playerController);
	playerController->SetCutScene(cutScene);

	mMainCamera = std::make_shared<CGameObject>();
	mMainCamera->SetTag("MainCamera");
	mMainCamera->SetName("MainCamera");
	mMainCamera->SetStatic(false);

	Vec2 rtSize = INSTANCE(CDX12Manager).GetRenderTargetSize();
	auto camera = mMainCamera->AddComponent<CCamera>();
	camera->mCameraName = "MainCamera";
	camera->SetViewport(0, 0, rtSize.x, rtSize.y);
	camera->SetScissorRect(0, 0, rtSize.x, rtSize.y);

#ifdef REVERSE_Z
	camera->GenerateReverseZPerspectiveProjectionMatrix(0.1f, 100.f, 60.f);
#elif // REVERSE_Z
	camera->GeneratePerspectiveProjectionMatrix(1.f, 100.f, 60.f);
#endif // REVERSE_Z

	auto playerFollower = mMainCamera->AddComponent<CThirdPersonCamera>();
	playerFollower->SetTarget(mPlayer);
	playerController->SetCamera(camera);
	cutScene->SetThirdPersonCamera(playerFollower);

	return true;
}

void ServerManager::RegisterPlayerInScene(class CScene* scene)
{
	scene->AddObjectImmediately(mPlayer);
	scene->AddObjectImmediately(mMainCamera);

	for (auto& pair : mOtherPlayers) {
		scene->AddObjectImmediately(pair.second);
	}
}

void ServerManager::AddNewPlayer(int id, Vec3 pos)
{
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	auto player = std::make_shared<CGameObject>();
	player->SetTag("Player");
	player->SetName("Player" + std::to_string(id));
	player->SetRenderLayer(RENDER_LAYER::Opaque);
	player->SetStatic(false);
	player->GetTransform()->SetLocalPosition(pos);
	player->mID = id;
	mOtherPlayers[id] = player;
}

void ServerManager::Recv_Packet()
{
	DWORD recv_flag = 0;

	OVER_PLUS* sdata = new OVER_PLUS();
	sdata->_over.hEvent = reinterpret_cast<HANDLE>(this);

	wsaover.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (wsaover.hEvent == nullptr) {
		print_error("CreateEvent failed", GetLastError());
	}
	wsaover.hEvent = reinterpret_cast<HANDLE>(this);

	int res = WSARecv(server_socket, &sdata->_wsabuf, 1, 0, &recv_flag, &sdata->_over, recv_callback);
	if (0 != res) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			print_error("Recv_Packet - WSARecv", WSAGetLastError());
	}
}

void CALLBACK ServerManager::recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
	OVER_PLUS* over = reinterpret_cast<OVER_PLUS*>(pwsaover);

	ServerManager* sm = reinterpret_cast<ServerManager*>(pwsaover->hEvent);
	if (sm == nullptr) {
		std::cout << "DDd" << std::endl;
		return;
	}
	char* buf = over->_wsabuf.buf;
	char recv_buf[CHAT_SIZE * 2];

	if (sm->save_data_size > 0) { 
		memcpy(recv_buf, sm->save_buf, sm->save_data_size);
		memcpy(&recv_buf[sm->save_data_size], buf, sm->one_packet_size - sm->save_data_size);
		buf += sm->one_packet_size - sm->save_data_size;
		recv_size -= sm->one_packet_size - sm->save_data_size;
		sm->save_data_size = 0;
		sm->Using_Packet(recv_buf);
	}

	while (1) {
		if (recv_size == 0) break; 
		WORD* byte = reinterpret_cast<WORD*>(buf);
		//cout << "recv_size : " << recv_size << ", one_packet_size : " << *byte << endl;
		sm->one_packet_size = *byte; 
		if (sm->one_packet_size > recv_size) { 
			memcpy(sm->save_buf, buf, recv_size);
			sm->save_data_size = recv_size;
			break;
		}
		memcpy(recv_buf, buf, sm->one_packet_size);
		sm->Using_Packet(recv_buf);
		buf += sm->one_packet_size;
		recv_size -= sm->one_packet_size;
	}

	//delete over;
}

void ServerManager::Recv_Loop()
{
	while (!RenderOK) {
		Recv_Packet();

		SleepEx(1, true);
	}
}

void ServerManager::Using_Packet(char* packet_ptr)
{
	switch (packet_ptr[2])
	{
	case SC_LOGIN_INFO: {
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(packet_ptr);

		clientID = packet->id;
		cout << clientID << endl;
		mIsLoggedIn = true;
		break;
	}
	case SC_LOGIN_FAIL: {
		cout << "Login Fail--!!!!" << endl;
		break;
	}
	case SC_ADD_OBJECT: {
		SC_ADD_OBJECT_PACKET* packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(packet_ptr);

		break;
	}
	case SC_ADD_PLAYER: {
		SC_ADD_PLAYER_PACKET* packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(packet_ptr);
		std::string objName[3] = { "Archer", "Fighter", "Mage" };
		auto obj = RESOURCE.GetPrefab(objName[packet->player_class]);
		if (!obj) {
			std::cout << "obj is nullptr" << std::endl;
			break;
		}

		std::shared_ptr<CGameObject> player{};
		if (clientID == packet->id) {
			player = mPlayer;
			player->GetCutScene()->SetClass((PLAYER_CLASS)packet->player_class);
			RenderOK = 1;
		}
		else {
			AddNewPlayer(packet->id, { packet->x, packet->y, packet->z });
			player = mOtherPlayers[packet->id];
		}
		CGameObject::Instantiate(obj, player->GetTransform());
		player->SetActive(true);
		player->GetTransform()->SetLocalPosition({ packet->x, packet->y, packet->z });
		player->GetTransform()->SetLocalRotationY(packet->look_y);
		
		std::shared_ptr<CPlayerStateMachine> stateMachine{};
		if(packet->player_class == (UINT8)PLAYER_CLASS::ARCHER) {
			stateMachine = player->AddComponent<CArcherState>();
		}
		else if (packet->player_class == (UINT8)PLAYER_CLASS::FIGHTER) {
			stateMachine = player->AddComponent<CWarriorState>();
		}
		else if (packet->player_class == (UINT8)PLAYER_CLASS::MAGE) {
			stateMachine = player->AddComponent<CMageState>();
		}
		else {
			std::cout << "Unknown player class: " << (int)packet->player_class << std::endl;
			return;
		}
		
		stateMachine->SetState((UINT8)PLAYER_STATE::IDLE);
		player->SetStateMachine(stateMachine);

		auto shieldPrefab = RESOURCE.GetPrefab("Water_Shield");
		if (shieldPrefab) {
			auto shieldObj = CGameObject::Instantiate(shieldPrefab, player->GetTransform());
			shieldObj->SetRenderLayer(RENDER_LAYER::Transparent);
			shieldObj->GetTransform()->SetLocalPosition({ 0.f, 0.6f, 0.f });
			stateMachine->SetShield(shieldObj);
			stateMachine->ActivateShield(false); 
		}

		auto scene = INSTANCE(CSceneManager).GetCurScene();
		if (clientID != packet->id) {
			scene->AddObject(player);
		}
		break;
	}
	case SC_CHANGE_SCENE: {
		SC_CHANGE_SCENE_PACKET* packet = reinterpret_cast<SC_CHANGE_SCENE_PACKET*>(packet_ptr);
		SCENE_TYPE sceneType = (SCENE_TYPE)packet->change_scene;
		if (sceneType == INSTANCE(CSceneManager).GetCurSceneType()) break; // 이미 같은 씬이면 리턴
		
		INSTANCE(CSceneManager).RequestSceneChange((SCENE_TYPE)packet->change_scene);
		break;
	}
	case SC_ALL_PLAYERS_POS: {
		SC_ALL_PLAYERS_POS_PACKET* packet = reinterpret_cast<SC_ALL_PLAYERS_POS_PACKET*>(packet_ptr);

		for (int i = 0; i < 3; i++) {
			if (packet->clientId[i] == -1) break;
			std::shared_ptr<CGameObject> player{};
			if (clientID == packet->clientId[i]) player = mPlayer;
			else {
				auto it = mOtherPlayers.find(packet->clientId[i]);
				if (it != mOtherPlayers.end()) player = it->second;
			}
			if (!player) continue;
			player->GetTransform()->SetLocalPosition({ packet->x[i], packet->y[i], packet->z[i] });
			player->GetTransform()->SetLocalRotationY(packet->look_y[i]);
			auto playerState = player->GetStateMachine();
			
			if (playerState && playerState->GetState() != packet->state[i])
				playerState->SetState(packet->state[i]);
		}
		break;
	}
	case SC_DROP_ITEM: {
		SC_DROP_ITEM_PACKET* packet = reinterpret_cast<SC_DROP_ITEM_PACKET*>(packet_ptr);
		cout << "packet._pos : " << packet->x << ", " << packet->y << ", " << packet->z << endl;
		auto scene = INSTANCE(CSceneManager).GetCurScene();

		std::string objName[ITEM_TYPE::item_end] 
			= { "FireEnchant", "FireExplosion", "WaterHeal", "WaterShield", "GrassVine", 
			"GrassWeaken", "FirePiece", "WaterPiece", "GrassPiece" };
		auto item = RESOURCE.GetPrefab(objName[(int)packet->item_enum]);
		if (!item) {
			std::cout << "item is nullptr" << std::endl;
			break;
		}
		auto itemObj = CGameObject::Instantiate(item);
		itemObj->SetTag("Item");
		itemObj->SetRenderLayer(RENDER_LAYER::Transparent);
		itemObj->SetObjectType(OBJECT_TYPE::ITEM);
		itemObj->SetActive(true);
		itemObj->SetStatic(false);
		itemObj->GetTransform()->SetLocalPosition({ packet->x, packet->y, packet->z });
		auto movement = itemObj->AddComponent<CItemMovement>();
		movement->SetAmplitude(0.2f);
		movement->SetFrequency(1.f);
		movement->SetDirection({ 0.f, 1.f, 0.f });
		movement->SetTargetObject(mMainCamera);

		if (scene && scene->mIsActive) {
			itemObj->Awake();
			itemObj->Start();
		}
		
		itemObj->mID = packet->item_id;
		mItems[packet->item_id] = itemObj;
		scene->AddObjectImmediately(itemObj);

		break;
	}
	case SC_REMOVE_ITEM: {
		SC_REMOVE_ITEM_PACKET* packet = reinterpret_cast<SC_REMOVE_ITEM_PACKET*>(packet_ptr);

		if (packet->player_id == -1) {} // 단순 삭제면 바로 넘기기
		else mPlayer->GetPlayerController()->SetSkill((ITEM_TYPE)packet->item_type);

		auto scene = INSTANCE(CSceneManager).GetCurScene();
		scene->RemoveObject(mItems[packet->item_id]);
		mItems.erase(packet->item_id);

		cout << "삭제!";
		break;
	}
	case SC_USE_SKILL: {
		SC_USE_SKILL_PACKET* packet = reinterpret_cast<SC_USE_SKILL_PACKET*>(packet_ptr);
		

		break;
	}
	case SC_ADD_PROJECTILE: {
		SC_ADD_PROJECTILE_PACKET* packet = reinterpret_cast<SC_ADD_PROJECTILE_PACKET*>(packet_ptr);
		auto scene = INSTANCE(CSceneManager).GetCurScene();

		std::string objName[(int)PROJECTILE_TYPE::PROJECTILE_END] 
			= { "Arrow", "FireBall", "IceBall", "GrassBall", "MagicBall" };
		auto projectile = RESOURCE.GetPrefab(objName[(int)packet->projectile_type]);
		if (!projectile) {
			std::cout << "projectile is nullptr" << std::endl;
			break;
		}
		auto projectileObj = CGameObject::Instantiate(projectile);
		projectileObj->SetTag("Projectile");
		projectileObj->SetRenderLayer(RENDER_LAYER::Opaque);
		if (packet->user_friendly)
			projectileObj->SetObjectType(OBJECT_TYPE::PLAYER_PROJECTILE);
		else
			projectileObj->SetObjectType(OBJECT_TYPE::ENEMY_PROJECTILE);
		projectileObj->SetActive(true);
		projectileObj->SetStatic(false);
		projectileObj->GetTransform()->SetLocalPosition({ packet->x, packet->y, packet->z });
		Quaternion local_rot = Quaternion::LookRotation(Vec3(packet->dir_x, packet->dir_y, packet->dir_z));
		projectileObj->GetTransform()->SetLocalRotation(local_rot);

		projectileObj->mID = packet->projectile_id;
		mProjectiles[packet->projectile_id] = projectileObj;
		scene->AddObject(projectileObj);
		break;
	}
	case SC_PROJECTILE_POS: {
		SC_PROJECTILE_POS_PACKET* packet = reinterpret_cast<SC_PROJECTILE_POS_PACKET*>(packet_ptr);
		
		if(mProjectiles.contains(packet->projectile_id)) {
			//Quaternion local_rot = Quaternion::LookRotation(Vec3(packet->dir_x, packet->dir_y, packet->dir_z));
			//mProjectiles[packet->projectile_id]->GetTransform()->SetLocalRotation(local_rot);
			mProjectiles[packet->projectile_id]->GetTransform()->SetLocalPosition({ packet->x, packet->y, packet->z });
		}
		break;
	}
	case SC_ADD_MONSTER: {
		SC_ADD_MONSTER_PACKET* packet = reinterpret_cast<SC_ADD_MONSTER_PACKET*>(packet_ptr);
		auto scene = INSTANCE(CSceneManager).GetCurScene();
		std::string objName[(int)ENEMY_TYPE::ENEMY_END]
			= { "GrassSmall", "GrassBig", "FireSmall", "FireBig", "WaterSmall", "WaterBig" };
		ENEMY_TYPE enumType = (ENEMY_TYPE)packet->monster_type;
		auto monster = RESOURCE.GetPrefab(objName[(int)enumType]);
		if (!monster) {
			std::cout << "monster is nullptr" << std::endl;
			break;
		}
		auto monsterObj = CGameObject::Instantiate(monster);
		monsterObj->SetTag("monster");
		monsterObj->SetRenderLayer(RENDER_LAYER::Opaque);
		monsterObj->SetActive(true);
		monsterObj->SetStatic(false);
		monsterObj->SetObjectType(OBJECT_TYPE::ENEMY);
		monsterObj->GetTransform()->SetLocalPosition({ packet->x, packet->y, packet->z });
		monsterObj->GetTransform()->SetLocalRotationY(packet->look_y);

		std::shared_ptr<CEnemyState> stateMachine{};
		switch (enumType)
		{
		case ENEMY_TYPE::GRASS_SMALL:
			stateMachine = monsterObj->AddComponent<CGrassSmallState>();
			break;
		case ENEMY_TYPE::GRASS_BIG:
			stateMachine = monsterObj->AddComponent<CGrassBigState>();
			break;
		default:
			break;
		}
		if (stateMachine) {
			stateMachine->SetState((UINT8)MONSTER_STATE::UNDERGROUND);
			monsterObj->SetStateMachine(stateMachine);
		}

		monsterObj->mID = packet->monster_id;
		mEnemies[packet->monster_id] = monsterObj;
		scene->AddObject(monsterObj);

		cout << "Monster Added! ID : " << packet->monster_id << "type : " << (int)packet->monster_type << endl;
		break;
	}
	case SC_MONSTER_POS: {
		SC_MONSTER_POS_PACKET* packet = reinterpret_cast<SC_MONSTER_POS_PACKET*>(packet_ptr);
		if (mEnemies.contains(packet->monsterId)) {
			mEnemies[packet->monsterId]->GetTransform()->SetLocalPosition({ packet->x, packet->y, packet->z });
			
			Quaternion local_rot = Quaternion::LookRotation(Vec3(packet->look_x, packet->look_y, packet->look_z));
			mEnemies[packet->monsterId]->GetTransform()->SetLocalRotation(local_rot); 
			// cout << "몬스터 look : " << packet->look_x << ", " << packet->look_y << ", " << packet->look_z << endl;
			
			auto monsterState =  mEnemies[packet->monsterId]->GetStateMachine();

			cout << "Monster State : " << (int)monsterState->GetState() << endl;

			if (monsterState && monsterState->GetState() != packet->monster_state){
				monsterState->SetState(packet->monster_state);
				cout << "몬스터 상태" << monsterState->GetState() << endl;	
			}
		}
		break;
	}
	case SC_REMOVE_MONSTER: {
		SC_REMOVE_MONSTER_PACKET* packet = reinterpret_cast<SC_REMOVE_MONSTER_PACKET*>(packet_ptr);
		auto scene = INSTANCE(CSceneManager).GetCurScene();
		scene->RemoveObject(mEnemies[packet->monster_id]);
		mEnemies.erase(packet->monster_id);
		break;
	}
	case SC_REMOVE_PROJECTILE: {
		SC_REMOVE_PROJECTILE_PACKET* packet = reinterpret_cast<SC_REMOVE_PROJECTILE_PACKET*>(packet_ptr);
		auto scene = INSTANCE(CSceneManager).GetCurScene();
		if (!mProjectiles.contains(packet->projectile_id)) break;
		scene->RemoveObject(mProjectiles[packet->projectile_id]);
		mProjectiles.erase(packet->projectile_id);
		break;
	}
	default:
		break;
	}
}

void ServerManager::Send_Packet(void* packet)
{
	OVER_PLUS* sdata = new OVER_PLUS{ reinterpret_cast<char*>(packet) };
	int sed = WSASend(server_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, send_callback);
	if (0 != sed) {
		int err_no = WSAGetLastError();
		// ���� ��ģ i/o �۾��� �����ϰ� �ֽ��ϴ�. ��� ������ �� ������
		if (WSA_IO_PENDING != err_no)
			print_error("Send_Packet - WSASend", WSAGetLastError());
	}
}
void CALLBACK ServerManager::send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
	OVER_PLUS* over = reinterpret_cast<OVER_PLUS*>(pwsaover);
	delete over;
}

void ServerManager::print_error(const char* msg, int err_no)
{
	WCHAR* msg_buf{};
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
	std::cout << msg;
	std::wcout << L"Because... : " << msg_buf;
	while (true);
	LocalFree(msg_buf);
}