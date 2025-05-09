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

void ServerManager::Client_Login()
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

	WSAOVERLAPPED wsaover;
	ZeroMemory(&wsaover, sizeof(wsaover));

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
	mPlayer->SetRenderLayer("Opaque");
	mPlayer->SetActive(false);
	mPlayer->SetStatic(false);

	auto playerController = mPlayer->AddComponent<CPlayerController>();

	mMainCamera = std::make_shared<CGameObject>();

	Vec2 rtSize = INSTANCE(CDX12Manager).GetRenderTargetSize();
	auto camera = mMainCamera->AddComponent<CCamera>();
	camera->SetViewport(0, 0, rtSize.x, rtSize.y);
	camera->SetScissorRect(0, 0, rtSize.x, rtSize.y);
	camera->GeneratePerspectiveProjectionMatrix(1.f, 100.f, 60.f);
	mMainCamera->SetTag("MainCamera");
	mMainCamera->SetName("MainCamera");
	mMainCamera->SetActive(false);
	mMainCamera->SetStatic(false);

	auto playerFollower = mMainCamera->AddComponent<CThirdPersonCamera>();
	playerFollower->SetTarget(mPlayer);
	playerController->SetCamera(camera);

	return true;
}

void ServerManager::RegisterPlayerInScene(class CScene* scene)
{
	mPlayer->SetActive(true);
	mMainCamera->SetActive(true);
	scene->AddObject(mPlayer);
	scene->AddObject(mMainCamera);

	auto camera = mMainCamera->GetComponent<CCamera>();
	scene->AddCamera(camera);

	for (auto& pair : mOtherPlayers) {
		scene->AddObject(pair.second);
	}
}

void ServerManager::AddNewPlayer(int id, Vec3 pos)
{
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	auto player = std::make_shared<CGameObject>();
	player->SetTag("Player");
	player->SetName("Player" + std::to_string(id));
	player->SetRenderLayer("Opaque");
	player->SetActive(false);
	player->SetStatic(false);
	player->GetTransform()->SetLocalPosition(pos);

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

		auto playerController = mPlayer->AddComponent<CPlayerController>();
		mPlayer->SetPlayerController(playerController);
		mPlayer->GetTransform()->SetLocalPosition({ 10, 5, 10 });

		cout << clientID << endl;

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
		std::string objName[3] = { "Player_Archer", "Player_Fighter", "Player_Mage" };
		auto obj = RESOURCE.GetPrefab(objName[packet->player_class]);
		if (!obj) {
			std::cout << "obj is nullptr" << std::endl;
			break;
		}

		std::shared_ptr<CGameObject> player{};
		if (clientID == packet->id) {
			player = mPlayer;
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

		auto scene = INSTANCE(CSceneManager).GetCurScene();
		if (scene && clientID != packet->id) {
			player->Awake();
			player->Start();
			scene->AddObject(player);
		}

		break;
	}
	case SC_ALL_PLAYERS_POS: {
		SC_ALL_PLAYERS_POS_PACKET* packet = reinterpret_cast<SC_ALL_PLAYERS_POS_PACKET*>(packet_ptr);

		for (int i = 0; i < 3; i++) {
			if (packet->clientId[i] == -1) break;
			if (clientID == packet->clientId[i]) {
				mPlayer->GetTransform()->SetLocalPosition({ packet->x[i], packet->y[i], packet->z[i] });
				mPlayer->GetTransform()->SetLocalRotationY(packet->look_y[i]);
			}
			else {
				auto it = mOtherPlayers.find(packet->clientId[i]);
				if (it != mOtherPlayers.end()) {
					mOtherPlayers[packet->clientId[i]]->GetTransform()->SetLocalPosition({ packet->x[i], packet->y[i], packet->z[i] });
					mOtherPlayers[packet->clientId[i]]->GetTransform()->SetLocalRotationY(packet->look_y[i]);
				}
			}
		}
		break;
	}
	case SC_DROP_ITEM: {
		SC_DROP_ITEM_PACKET* packet = reinterpret_cast<SC_DROP_ITEM_PACKET*>(packet_ptr);
		auto scene = INSTANCE(CSceneManager).GetCurScene();

		std::string objName[3] = { "Item_Skill1", "Item_Skill1", "Item_Skill3" };
		auto item = RESOURCE.GetPrefab(objName[(int)packet->item_enum]);
		if (!item) {
			std::cout << "item is nullptr" << std::endl;
			break;
		}
		auto itemObj = CGameObject::Instantiate(item);
		itemObj->SetTag("Item");
		itemObj->SetRenderLayer("Opaque");
		itemObj->SetObjectType(OBJECT_TYPE::ITEM);
		itemObj->SetActive(true);
		itemObj->SetStatic(false);
		itemObj->GetTransform()->SetLocalPosition({ packet->x, packet->y, packet->z });
		itemObj->Awake();
		itemObj->Start();

		mItems[packet->item_id] = itemObj;
		scene->AddObject(itemObj);

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
	std::wcout << L"\t���� : " << msg_buf;
	while (true);
	LocalFree(msg_buf);
}