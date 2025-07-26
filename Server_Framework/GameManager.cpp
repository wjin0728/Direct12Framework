#include "GameManager.h"

GameManager::GameManager()
{
	terrain[(int)S_SCENE_TYPE::LOBBY].SetScale(45, 20, 45);
	terrain[(int)S_SCENE_TYPE::LOBBY].SetResolution(513);
	terrain[(int)S_SCENE_TYPE::LOBBY].SetNavMapResolution(terrain[(int)S_SCENE_TYPE::LOBBY].GetResolution() * 2);
	terrain[(int)S_SCENE_TYPE::LOBBY].LoadHeightMap("LobbyTerrainHeightmap");
	terrain[(int)S_SCENE_TYPE::LOBBY].LoadNavMap("LobbyTerrainNavMask");
	terrain[(int)S_SCENE_TYPE::MAINSTAGE1].SetScale(100.f, 598.9f, 100.f);
	terrain[(int)S_SCENE_TYPE::MAINSTAGE1].SetResolution(513);
	terrain[(int)S_SCENE_TYPE::MAINSTAGE1].SetNavMapResolution(terrain[(int)S_SCENE_TYPE::MAINSTAGE1].GetResolution() * 2);
	terrain[(int)S_SCENE_TYPE::MAINSTAGE1].LoadHeightMap("Battle1TerrainHeightmap");
	terrain[(int)S_SCENE_TYPE::MAINSTAGE1].LoadNavMap("Battle1TerrainNavMask");
	terrain[(int)S_SCENE_TYPE::MAINSTAGE2].SetScale(64.f, 600.9f, 64.f);
	terrain[(int)S_SCENE_TYPE::MAINSTAGE2].SetResolution(513);
	terrain[(int)S_SCENE_TYPE::MAINSTAGE2].SetNavMapResolution(terrain[(int)S_SCENE_TYPE::MAINSTAGE1].GetResolution() * 2);
	terrain[(int)S_SCENE_TYPE::MAINSTAGE2].LoadHeightMap("Battle2TerrainHeightmap");
	terrain[(int)S_SCENE_TYPE::MAINSTAGE2].LoadNavMap("Battle2TerrainNavMask");
	terrain[(int)S_SCENE_TYPE::MAINSTAGE3].SetScale(64.f, 600.f, 64.f);
	terrain[(int)S_SCENE_TYPE::MAINSTAGE3].SetResolution(513);
	terrain[(int)S_SCENE_TYPE::MAINSTAGE3].SetNavMapResolution(terrain[(int)S_SCENE_TYPE::MAINSTAGE1].GetResolution() * 2);
	terrain[(int)S_SCENE_TYPE::MAINSTAGE3].LoadHeightMap("Battle3TerrainHeightmap");
	terrain[(int)S_SCENE_TYPE::MAINSTAGE3].LoadNavMap("Battle3TerrainNavMask");
	cout << "Map loaded.\n";

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (server_socket == INVALID_SOCKET) {
		std::cerr << "WSASocket failed: " << WSAGetLastError() << "\n";
		WSACleanup();
		exit(1);
	}

	std::cout << "Server socket created.\n";
	S_Bind_Listen();
	std::cout << "Server initialized.\n";

	S_Accept();

	scene_type = S_SCENE_TYPE::LOBBY;

	spawn_points[(int)S_SCENE_TYPE::LOBBY] = Vec3(4.803865f, 0.4409764f, 8.894886f);
	spawn_points[(int)S_SCENE_TYPE::MAINSTAGE1] = Vec3(45.2f, 4.2f, 42.f);
	spawn_points[(int)S_SCENE_TYPE::MAINSTAGE2] = Vec3(5.075171f, 2.164612f, 25.88103f);
	spawn_points[(int)S_SCENE_TYPE::MAINSTAGE3] = Vec3(26.92197f, 1.299845, 6.873069);
}
GameManager::~GameManager()
{
	closesocket(server_socket);
	WSACleanup();
	for (auto& th : workerThreads) {
		if (th.joinable()) th.join();
	}
}

void GameManager::S_Bind_Listen()
{
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	std::cout << "Binding to port " << PORT_NUM << "\n";
	if (::bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
		std::cerr << "Bind failed: " << WSAGetLastError() << "\n";
		closesocket(server_socket);
		WSACleanup();
		exit(1);
	}
	if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed: " << WSAGetLastError() << "\n";
		closesocket(server_socket);
		WSACleanup();
		exit(1);
	}
}
void GameManager::S_Accept()
{
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server_socket), h_iocp, 9999, 0);
	client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	accept_over._comp_type = OP_ACCEPT;
	AcceptEx(server_socket, client_socket, accept_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &accept_over._over);
}

void GameManager::StartWorkerThreads() {
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i) {
		workerThreads.emplace_back(&GameManager::Worker_thread, this);
	}
}

void GameManager::Worker_thread()
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_PLUS* ex_over = reinterpret_cast<OVER_PLUS*>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				Disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		if ((0 == num_bytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
			Disconnect(static_cast<int>(key));
			if (ex_over->_comp_type == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			int client_id = Get_new_Client_id();
			if (client_id != -1) {
				{
					lock_guard<mutex> ll(clients[ServerNumber][client_id]._s_lock);
					clients[ServerNumber][client_id]._state = ST_ALLOC;
				}
				clients[ServerNumber][client_id]._id = client_id;
				clients[ServerNumber][client_id]._prev_remain = 0;
				clients[ServerNumber][client_id]._socket = client_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket), h_iocp, client_id, 0);
				clients[ServerNumber][client_id].do_recv();
				client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else {
				cout << "Max user exceeded.\n";
			}
			ZeroMemory(&accept_over._over, sizeof(accept_over._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(server_socket, client_socket, accept_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &accept_over._over);
			break;
		}
		case OP_RECV: {
			int remain_data = num_bytes + clients[ServerNumber][key]._prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				WORD* byte = reinterpret_cast<WORD*>(p);
				int packet_size = *byte;
				if (packet_size <= remain_data) {
					Process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[ServerNumber][key]._prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[ServerNumber][key].do_recv();
			break;
		}
		case OP_SEND: {
			delete ex_over;
			break;
		}
		}
	}

	CoUninitialize();
}

void GameManager::Disconnect(int c_id)
{
	closesocket(clients[ServerNumber][c_id]._socket);

	lock_guard<mutex> ll(clients[ServerNumber][c_id]._s_lock);
	clients[ServerNumber][c_id]._state = ST_FREE;
}

int GameManager::Get_new_Client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		lock_guard <mutex> ll{ clients[ServerNumber][i]._s_lock };
		if (clients[ServerNumber][i]._state == ST_FREE)
			return i;
	}
	return -1;
}

void GameManager::Process_packet(int c_id, char* packet)
{
	switch (packet[2]) {
	case CS_GAME_SERVER_LOGIN: {
		CS_GAME_SERVER_LOGIN_PACKET* p = reinterpret_cast<CS_GAME_SERVER_LOGIN_PACKET*>(packet); {
			lock_guard<mutex> ll{ clients[ServerNumber][c_id]._s_lock };
			clients[ServerNumber][c_id]._state = ST_INGAME;
		}

		//clients[ServerNumber][c_id]._player.SetClass((S_PLAYER_CLASS)p->player_class);

		clients[ServerNumber][c_id]._player._pos = spawn_points[(int)scene_type];
		cout << "login : " << c_id << endl;
		break;
	}
	case CS_CHAT: {
		CS_CHAT_PACKET* p = reinterpret_cast<CS_CHAT_PACKET*>(packet);

		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second.send_chat_packet(c_id, p->mess);
		}

		std::cout << p->mess << std::endl;
		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);

		Vec3 moveDir = Vec3::Zero;
		Vec3 local_lookDir = Vec3(p->look_x, p->look_y, p->look_z);
		local_lookDir.y = 0.f;
		local_lookDir.Normalize();
		Vec3 right_dir = local_lookDir.Cross(Vec3::Up); 
		right_dir.y = 0.f;
		right_dir.Normalize();

		if (p->dir & KEY_FLAG::KEY_W) moveDir += local_lookDir;
		if (p->dir & KEY_FLAG::KEY_S) moveDir -= local_lookDir;
		if (p->dir & KEY_FLAG::KEY_D) moveDir -= right_dir;
		if (p->dir & KEY_FLAG::KEY_A) moveDir += right_dir;

		if (moveDir.LengthSquared() > 0.0001f) {
			moveDir.Normalize();
			clients[ServerNumber][c_id]._player._velocity = moveDir * 3.f;
		}
		else {
			clients[ServerNumber][c_id]._player._velocity = Vec3::Zero;
		}

		break;
	}
	case CS_SKILL_TARGET: {
		CS_SKILL_TARGET_PACKET* p = reinterpret_cast<CS_SKILL_TARGET_PACKET*>(packet);

		if (S_FIRE_EXPLOSION == p->skill_enum) {
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._state != ST_INGAME) continue;
				Monsters[ServerNumber][p->target_id].TakeDamage(10, true);
				cl.second.send_hp_packet((S_OBJECT_TYPE)S_ENEMY, p->target_id, Monsters[ServerNumber][p->target_id]._hp, 0);
			}
		}
		else if (S_GRASS_VINE == p->skill_enum) {}
		break;
	}
	case CS_SKILL_NONTARGET: {
		CS_SKILL_NONTARGET_PACKET* p = reinterpret_cast<CS_SKILL_NONTARGET_PACKET*>(packet);
		std::cout << "CS_SKILL_NONTARGET_PACKET skill_enum : " << p->skill_enum << std::endl;

		if (S_FIRE_ENCHANT == p->skill_enum) {
			clients[ServerNumber][c_id]._player._on_FireEnchant = true;
		}
		else if (S_WATER_HEAL == p->skill_enum) {
			for (auto& cl : clients[ServerNumber])
				cl.second._player._hp = min((cl.second._player._hp + WATER_HEAL_AMT), cl.second._player.PlayerMaxHp());
		}
		else if (S_WATER_SHIELD == p->skill_enum) {
			for (auto& cl : clients[ServerNumber]) {
				cl.second._player._barrier = 2; // 워터실드!!!!!!!
				cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, c_id, cl.second._player._hp, cl.second._player._barrier);
				cl.second.send_use_skill_packet(S_WATER_SHIELD, c_id);
				std::cout << "Water Shield activated for player " << c_id << std::endl;
			}
		}
		else if (S_GRASS_WEAKEN == p->skill_enum) {
			clients[ServerNumber][c_id]._player._on_GrassWeaken = true;
		}
		break;
	}
	case CS_ULTIMATE_SKILL: {
		CS_ULTIMATE_SKILL_PACKET* p = reinterpret_cast<CS_ULTIMATE_SKILL_PACKET*>(packet);
		break;
	}
	case CS_000: {
		CS_000_PACKET* p = reinterpret_cast<CS_000_PACKET*>(packet);

		switch (p->key)
		{ 
		// 아이템 생성 
		case 0: {
			items[ServerNumber][Item_cnt[ServerNumber]].SetPosition(clients[ServerNumber][c_id]._player._pos.x + 3,
				clients[ServerNumber][c_id]._player._pos.y + 0.3, clients[ServerNumber][c_id]._player._pos.z);
			items[ServerNumber][Item_cnt[ServerNumber]].SetItemType(S_ITEM_TYPE::S_FIRE_ENCHANT);
			items[ServerNumber][Item_cnt[ServerNumber]].LocalTransform();

			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._state != ST_INGAME) continue;
				cl.second.send_drop_item_packet(items[ServerNumber][Item_cnt[ServerNumber]], Item_cnt[ServerNumber]);
			}

			Item_cnt[ServerNumber]++;
			break;
		}
		// 몬스터 생성
		case 1: {
			InitializeMonsters(scene_type);
			break;
		}
		// 씬 전환
		case 2: {
			ChangeScene((uint8_t)S_SCENE_TYPE::LOBBY);
			break;
		}
		// 씬 전환
		case 3: {
			ChangeScene((uint8_t)S_SCENE_TYPE::MAINSTAGE1);
			break;
		}
		// 씬 전환
		case 4: {
			ChangeScene((uint8_t)S_SCENE_TYPE::MAINSTAGE2);
			break;
		}
		case 5: {
			ChangeScene((uint8_t)S_SCENE_TYPE::MAINSTAGE3);
			break;
		}
		default:
			break;
		} 
		break;
	}
	case CS_CHANGE_SCENE: {
		CS_CHANGE_SCENE_PACKET* p = reinterpret_cast<CS_CHANGE_SCENE_PACKET*>(packet);

		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			//cl.second.send_change_scene_packet(p->change_scene);
		}
		break;
	}
	case CS_CHANGE_STATE: {
		CS_CHANGE_STATE_PACKET* p = reinterpret_cast<CS_CHANGE_STATE_PACKET*>(packet);

		if (p->state != (uint8_t)clients[ServerNumber][p->id]._player._state)
			clients[ServerNumber][p->id]._player.SetState(p->state);
		break;
	}
	case CS_ATTACK: {
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		auto& player = clients[ServerNumber][c_id]._player;
		Vec3 direction{};

		switch (clients[ServerNumber][p->id]._player._class)
		{
		case S_PLAYER_CLASS::FIGHTER: {
			direction = player._velocity;
			for (auto& mon : Monsters[ServerNumber]) {
				if (mon.second._remove || mon.second._state == S_MONSTER_STATE::UNDERGROUND || mon.second._state == S_MONSTER_STATE::DEATH || mon.second._state == S_MONSTER_STATE::SPAWN) continue; // 몬스터가 제거된 경우는 패스
				mon.second.LocalTransform();
				if (player.OnFighterBasicAttack(mon.second._boundingbox)) {
					mon.second.TakeDamage(5, false);
					for (auto& cl : clients[ServerNumber]) {
						if (cl.second._state != ST_INGAME) continue;
						cl.second.send_hp_packet((S_OBJECT_TYPE)S_ENEMY, mon.first, mon.second._hp, 0);
					}
				}
			}
			break;
		}
		case S_PLAYER_CLASS::ARCHER: {
			cout << "Archer CS_ATTACK\n";
			Projectile proj{ 1, S_PROJECTILE_TYPE::ARROW };

			proj._pos = player._pos;
			proj._pos.y += 0.4f;

			direction.x = sin(player._look_dir.y * degToRad); // 1.0
			direction.y = 0.0f;
			direction.z = cos(player._look_dir.y * degToRad); // 0.0
			proj._velocity = direction/3;

			Projectiles[ServerNumber].insert({ Projectile_cnt[ServerNumber], proj });
			for (auto& cl : clients[ServerNumber]) {
				cl.second.send_add_projectile_packet(proj, Projectile_cnt[ServerNumber]);
			}
			Projectile_cnt[ServerNumber]++;
			break;
		}
		case S_PLAYER_CLASS::MAGE: {
			Projectile proj{ 1, S_PROJECTILE_TYPE::MAGIC_BALL };

			proj._pos = player._pos;
			proj._pos.y += 0.4f;

			direction.x = sin(player._look_dir.y * degToRad); // 1.0
			direction.y = 0.0f;
			direction.z = cos(player._look_dir.y * degToRad); // 0.0
			proj._velocity = direction / 4.f;

			Projectiles[ServerNumber].insert({ Projectile_cnt[ServerNumber], proj });
			for (auto& cl : clients[ServerNumber]) {
				cl.second.send_add_projectile_packet(proj, Projectile_cnt[ServerNumber]);
			}
			Projectile_cnt[ServerNumber]++;
			break;
		}
		default:
			break;
		}
		//Quaternion targetRot = Quaternion::LookRotation(direction);
		//Vec3 angle = Vec3::GetAngleToQuaternion(targetRot) * radToDeg;
		//clients[ServerNumber][p->id]._player._rotation = targetRot;
		//clients[ServerNumber][p->id]._player.SetLookDir(angle);
		break;
	}
	case CS_CLICK_BUTTON: {
		CS_CLICK_BUTTON_PACKET* p = reinterpret_cast<CS_CLICK_BUTTON_PACKET*>(packet);

		switch (p->button_type + (uint8_t)S_BUTTON_TYPE::ARCHER)
		{
		case (uint8_t)S_BUTTON_TYPE::ARCHER:
		case (uint8_t)S_BUTTON_TYPE::FIGHTER:
		case (uint8_t)S_BUTTON_TYPE::MAGE: {
			if (!IsClassOK((S_PLAYER_CLASS)p->button_type)) break;

			clients[ServerNumber][c_id]._player.SetClass((S_PLAYER_CLASS)(p->button_type));

			// 지금 login한 클라이언트 정보 -> 다른 클라이언트에게 전송
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._state != ST_INGAME) continue;
				cl.second.send_add_player_packet(&clients[ServerNumber][c_id]);
			}
			// 다른 클라이언트 정보 -> 지금 login한 클라이언트에게 전송
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._state != ST_INGAME) continue;
				if (cl.first == c_id) continue;
				clients[ServerNumber][c_id].send_add_player_packet(&cl.second);
			}

			break;
		}
		default:
			break;
		}

		clients[ServerNumber][c_id]._player.SetState((UINT8)S_PLAYER_STATE::IDLE);
		break;
	}
	}
}

bool GameManager::CanMove(float x, float z)
{
	if (terrain[(int)scene_type].mNavMapData.empty()) {
		return false;
	}
	float localX = x - terrain[(int)scene_type].GetOffset().x;
	float localZ = z - terrain[(int)scene_type].GetOffset().z;
	if (localX < 0.0f || localZ < 0.0f || localX >= terrain[(int)scene_type].GetScale().x || localZ >= terrain[(int)scene_type].GetScale().z) {
		return false;
	}
	float xIndex = localX / (terrain[(int)scene_type].GetScale().x / (terrain[(int)scene_type].GetNavMapResolution()));
	float zIndex = localZ / (terrain[(int)scene_type].GetScale().z / (terrain[(int)scene_type].GetNavMapResolution()));
	zIndex = terrain[(int)scene_type].GetNavMapResolution() - zIndex;
	xIndex = static_cast<int>(xIndex);
	zIndex = static_cast<int>(zIndex);

	int idx = xIndex + (zIndex * terrain[(int)scene_type].GetNavMapResolution());
	if (terrain[(int)scene_type].mNavMapData[idx] != 0) {
		return true;
	}

	return false;
}

void GameManager::Update()
{
	//int deadMonsterCnt = 0;
	//for (auto& ms : Monsters[ServerNumber]) {
	//	if (!ms.second._wave) {
	//		std::cout << "Monster " << ms.first << " is not in wave." << std::endl;
	//		deadMonsterCnt++;
	//	}
	//}

	for (auto& cl : clients[ServerNumber]) {
		auto& player = cl.second._player;

		if (cl.second._state == ST_INGAME) {
			//if (deadMonsterCnt == 3) {
			//	cl.second.send_make_potal_packet();
			//}
		}
		else continue;
		player.Update();

		if (player._class == S_PLAYER_CLASS::FIGHTER && player.currentState == &PlayerState::UltimateState::GetInstance()) {
			float terrainHeight = terrain[(int)scene_type].GetHeight(player._pos.x, player._pos.z);
			player._pos.y = terrainHeight + player._data;
		}

		// 플레이어 - 아이템 충돌 체크
		if (player._state == S_PLAYER_STATE::GATHERING && !items.empty()) {
			for (auto& it : items[ServerNumber]) {
				if (it.second._item_type > S_ITEM_TYPE::S_GRASS_WEAKEN)
					it.second.LocalTransform();

				Vec2 itemPos = Vec2(it.second._pos.x, it.second._pos.z);
				Vec2 playerPos = Vec2(player._pos.x, player._pos.z);
				if (Vec2::IsInRadius(itemPos, playerPos, 1.f)) {
					for (auto& cl : clients[ServerNumber]) {
						if (cl.second._state != ST_INGAME) continue;
						cl.second.send_remove_item_packet(it.first, cl.first, it.second._item_type);
					}
					cout << "cl : " << cl.first << "랑 item : " << it.first << " 충돌~!!!!!!!!!!!!!!!" << endl;
					items[ServerNumber].erase(it.first);
					break;
				}
			}
		}

		if (player.HasMoveInput()) {
			Vec3 newPos = player._pos + (player._velocity * TICK_INTERVAL);

			if (CanMove(newPos.x, newPos.z)) {
				player._pos = newPos;

				float terrainHeight = terrain[(int)scene_type].GetHeight(player._pos.x, player._pos.z);
				player._pos.y = terrainHeight;

				float rotationSpeed = 10.f;
				if (player._class != S_PLAYER_CLASS::ARCHER || player._state != S_PLAYER_STATE::RUNATTACK || !player._target) {
					if (player._velocity.LengthSquared() > 0.001f) {
						Quaternion targetRot = Quaternion::LookRotation(player._velocity);
						Quaternion rotation = player._rotation = Quaternion::Slerp(player._rotation, targetRot, rotationSpeed * TICK_INTERVAL);
						Vec3 angle = Vec3::GetAngleToQuaternion(rotation);
						player._look_dir.y = angle.y * radToDeg;
					}
				}
			}
			else {
				player._velocity = Vec3::Zero;
			}
		}
	}

	vector<int> erase_proj;

	for (auto& ms : Monsters[ServerNumber]) {
		auto& monster = ms.second;

		if (monster._remove) continue; // 몬스터가 제거된 경우는 패스
		else if (monster._state == S_MONSTER_STATE::DEATH && !monster._drop_item) {
			CreateItem(&monster);
			monster._drop_item = true;
		}

		// 이벤트 처리
		if (monster._state == S_MONSTER_STATE::ATTACK || monster._state == S_MONSTER_STATE::ATTACK2 || monster._state == S_MONSTER_STATE::PROJECTILE_ATTACK) {
			monster.HandleCallback(monster.mEventHandler[(int)monster._state]);
		}

		monster.Update();
		monster.AvoidCollision(Monsters[ServerNumber]);

		if (monster._state == S_MONSTER_STATE::RUN) {
			float terrainHeight = terrain[(int)scene_type].GetHeight(monster._pos.x, monster._pos.z);
			monster._pos.y = terrainHeight;
			
		}

		// 몬스터 - 투사체 충돌 체크
		if (monster._hp >= 0 && monster._state != S_MONSTER_STATE::UNDERGROUND && monster._state != S_MONSTER_STATE::DEATH) {
			for (auto& proj : Projectiles[ServerNumber]) {
				if (!proj.second._user_frinedly) continue; // 적이 쏜 projectile면 패스
				if (proj.second._remove) continue; // 투사체가 제거된 경우는 패스
				if (monster._boundingbox.Intersects(proj.second._boundingbox)) {
					monster.TakeDamage(proj.second._damage, false);
					for (auto& cl : clients[ServerNumber]) {
						if (cl.second._state != ST_INGAME) continue;
						cl.second.send_hp_packet((S_OBJECT_TYPE)S_ENEMY, ms.first, monster._hp, 0);
					}
					proj.second._remove = true; // 투사체 제거
				}
			}
		}
	}

	for (auto& proj : Projectiles[ServerNumber]) {
		proj.second.Update();
		if (abs(proj.second._pos.x) > 100.f || abs(proj.second._pos.z) > 100.f
			|| proj.second._remove) {
			erase_proj.emplace_back(proj.first);
		}
	}
	for (int i = 0; i < erase_proj.size(); ++i) {	
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second.send_remove_projectile_packet(erase_proj[i]);
		}
		Projectiles[ServerNumber].erase(erase_proj[i]);
	}

	SendAllPlayersPosPacket();
	SendAllProjectilesPosPacket();
	SendAllMonstersPosPacket();
	//SendAllItemsPosPacket();
}

void GameManager::SendAllPlayersPosPacket() {
	SC_ALL_PLAYERS_POS_PACKET packet;
	packet.size = sizeof(SC_ALL_PLAYERS_POS_PACKET);
	packet.type = SC_ALL_PLAYERS_POS;
	int cnt = 0;
	for (auto& cl : clients[ServerNumber]) {
		if (cl.second._state != ST_INGAME) { continue; }
		auto& player = cl.second._player;

		packet.clientId[cnt] = cl.second._id;
		packet.x[cnt] = player._pos.x;
		packet.y[cnt] = player._pos.y;
		packet.z[cnt] = player._pos.z;
		packet.look_y[cnt] = player._look_dir.y;

		cnt++;
	}
	for (int i = 0; i < 3; i++) {
		if (packet.clientId[i] == -1) {
			packet.x[i] = 0;
			packet.y[i] = 0;
			packet.z[i] = 0;
			packet.look_y[i] = 0;
		}
		else
			packet.state[i] = (uint8_t)clients[ServerNumber][i]._player._state;
	}
	for (auto& cl : clients[ServerNumber]) {
		if (cl.second._state != ST_INGAME) continue;

		//cout << packet.x[0] << " " << packet.y[0] << " " << packet.z[0] << endl;
		//cout << packet.look_y[0] << endl;
		//cout << (int)packet.state[0] << endl;

		cl.second.do_send(&packet);
	}
}
void GameManager::SendAllMonstersPosPacket() {
	SC_MONSTER_POS_PACKET packet;
	packet.size = sizeof(SC_MONSTER_POS_PACKET);
	packet.type = SC_MONSTER_POS;
	for (auto& ms : Monsters[ServerNumber]) {
		packet.monsterId = ms.first;
		packet.x = ms.second._pos.x;
		packet.y = ms.second._pos.y;
		packet.z = ms.second._pos.z;
		packet.look_x = ms.second._look_dir.x;
		packet.look_y = ms.second._look_dir.y;
		packet.look_z = ms.second._look_dir.z;
		packet.monster_state = (uint8_t)ms.second._state;

		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second.do_send(&packet);
		}
	}
}
void GameManager::SendAllItemsPosPacket() {
	for (auto& it : items[ServerNumber]) {
		SC_ITEM_POS_PACKET packet;
		packet.size = sizeof(SC_ITEM_POS_PACKET);
		packet.type = SC_ITEM_POS;
		packet.itemId = it.first;
		packet.x = it.second._pos.x;
		packet.y = it.second._pos.y;
		packet.z = it.second._pos.z;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second.do_send(&packet);
		}
	}
}
void GameManager::SendAllProjectilesPosPacket()
{
	SC_PROJECTILE_POS_PACKET packet;
	packet.size = sizeof(SC_PROJECTILE_POS_PACKET);
	packet.type = SC_PROJECTILE_POS;
	for (auto& proj : Projectiles[ServerNumber]) {
		packet.projectile_id = proj.first;
		packet.x = proj.second._pos.x;
		packet.y = proj.second._pos.y;
		packet.z = proj.second._pos.z;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second.do_send(&packet);
		}
	}
}

void GameManager::CreateItem(Monster* monster)
{
	float terrainHeight = terrain[(int)scene_type].GetHeight(monster->_pos.x, monster->_pos.z);

	items[ServerNumber][Item_cnt[ServerNumber]].SetPosition(monster->_pos.x, terrainHeight + 0.3, monster->_pos.z);
	//items[ServerNumber][Item_cnt[ServerNumber]].SetItemType(rand() % 2 ? S_ITEM_TYPE::S_FIRE_EXPLOSION : S_ITEM_TYPE::S_WATER_SHIELD);
	items[ServerNumber][Item_cnt[ServerNumber]].SetItemType(S_ITEM_TYPE::S_WATER_SHIELD);
	items[ServerNumber][Item_cnt[ServerNumber]].LocalTransform();

	for (auto& cl : clients[ServerNumber]) {
		if (cl.second._state != ST_INGAME) continue;
		cl.second.send_drop_item_packet(items[ServerNumber][Item_cnt[ServerNumber]], Item_cnt[ServerNumber]);
	}

	Item_cnt[ServerNumber]++;
}

void GameManager::InitializeMonsters(S_SCENE_TYPE scene_type)
{
	switch (scene_type) {
	case S_SCENE_TYPE::MAINSTAGE1: {
		InitializeGrassMonsters();
		break;
	}
	case S_SCENE_TYPE::MAINSTAGE2: {
		InitializeWaterMonsters();
		break;
	}
	case S_SCENE_TYPE::MAINSTAGE3: {
		InitializeFireMonsters();
		break;
	}
	}
}

void GameManager::InitializeGrassMonsters()
{
	{
		Monster ms{ S_ENEMY_TYPE::GRASS_SMALL };
		ms._pos = ms._spawn_pos = Vec3(50.f, 5.f, 50.f);
		ms._look_dir = ms._spawn_dir = Vec3(0.f, 0.f, 1.f);
		ms._hp = ms._max_hp = 50.f;
		ms.LocalTransform();
		for (auto& cl : clients[ServerNumber]) {
			ms._Player[cl.first] = &cl.second._player;
		}
		Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second._player._Monster[Monster_cnt[ServerNumber]] = &Monsters[ServerNumber][Monster_cnt[ServerNumber]];
			cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
		}

		auto func0 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.2f, 0.9f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 2.f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		auto func1 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 0.7f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;
				
				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 0.8f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", func1);
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK2, "Attack", func0);
		//Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::PROJECTILE_ATTACK, "Attack", func1);

		Monster_cnt[ServerNumber]++;
	}

	{
		Monster ms{ S_ENEMY_TYPE::GRASS_BIG };
		ms._pos = ms._spawn_pos = Vec3(55.f, 5.f, 50.f);
		ms._look_dir = ms._spawn_dir = Vec3(0.f, 0.f, 1.f);
		ms._hp = ms._max_hp = 100.f;
		ms.LocalTransform();
		for (auto& cl : clients[ServerNumber]) {
			ms._Player[cl.first] = &cl.second._player;
		}
		Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second._player._Monster[Monster_cnt[ServerNumber]] = &Monsters[ServerNumber][Monster_cnt[ServerNumber]];
			cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
		}

		auto func0 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 2.85f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 1.f)) {
					cl.second._player.TakeDamage(150);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		auto func1 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(1.25f, 1.25f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 2.f)) {
					cl.second._player.TakeDamage(150);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", func0);
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK2, "Attack", func1);
		//Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::PROJECTILE_ATTACK, "Attack", func1);

		Monster_cnt[ServerNumber]++;
	}

	{
		Monster ms{ S_ENEMY_TYPE::GRASS_SMALL };
		ms._pos = ms._spawn_pos = Vec3(60.f, 5.f, 50.f);
		ms._look_dir = ms._spawn_dir = Vec3(0.f, 0.f, 1.f);
		ms._hp = ms._max_hp = 50.f;
		ms.LocalTransform();
		for (auto& cl : clients[ServerNumber]) {
			ms._Player[cl.first] = &cl.second._player;
		}
		Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second._player._Monster[Monster_cnt[ServerNumber]] = &Monsters[ServerNumber][Monster_cnt[ServerNumber]];
			cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
		}

		auto func0 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.2f, 0.9f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 2.f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		auto func1 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 0.7f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 0.8f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", func1);
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK2, "Attack", func0);
		//Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::PROJECTILE_ATTACK, "Attack", func1);

		Monster_cnt[ServerNumber]++;
	}
}
void GameManager::InitializeWaterMonsters()
{
	{
		Monster ms{ S_ENEMY_TYPE::WATER_SMALL };
		ms._pos = ms._spawn_pos = Vec3(29.4f, 0.f, 35.6f);
		ms._look_dir = ms._spawn_dir = Vec3(0.f, 0.f, 1.f);
		ms._hp = ms._max_hp = 50.f;
		ms.LocalTransform();
		for (auto& cl : clients[ServerNumber]) {
			ms._Player[cl.first] = &cl.second._player;
		}
		Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second._player._Monster[Monster_cnt[ServerNumber]] = &Monsters[ServerNumber][Monster_cnt[ServerNumber]];
			cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
		}

		auto func0 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 1.7f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 0.85f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		auto func1 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 1.f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 1.f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", func1);
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK2, "Attack", func0);
		//Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::PROJECTILE_ATTACK, "Attack", func1);

		Monster_cnt[ServerNumber]++;
	}

	{
		Monster ms{ S_ENEMY_TYPE::WATER_BIG };
		ms._pos = ms._spawn_pos = Vec3(31.33f, 0.f, 33.24f);
		ms._look_dir = ms._spawn_dir = Vec3(0.f, 0.f, 1.f);
		ms._hp = ms._max_hp = 100.f;
		ms.LocalTransform();
		for (auto& cl : clients[ServerNumber]) {
			ms._Player[cl.first] = &cl.second._player;
		}
		Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second._player._Monster[Monster_cnt[ServerNumber]] = &Monsters[ServerNumber][Monster_cnt[ServerNumber]];
			cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
		}

		auto func0 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 2.5f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 1.9f)) {
					cl.second._player.TakeDamage(150);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		auto func1 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 0.f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 2.5f)) {
					cl.second._player.TakeDamage(150);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", func0);
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK2, "Attack", func1);
		//Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::PROJECTILE_ATTACK, "Attack", func1);

		Monster_cnt[ServerNumber]++;
	}

	{
		Monster ms{ S_ENEMY_TYPE::WATER_SMALL };
		ms._pos = ms._spawn_pos = Vec3(32.3f, 0.f, 29.f);
		ms._look_dir = ms._spawn_dir = Vec3(0.f, 0.f, 1.f);
		ms._hp = ms._max_hp = 50.f;
		ms.LocalTransform();
		for (auto& cl : clients[ServerNumber]) {
			ms._Player[cl.first] = &cl.second._player;
		}
		Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second._player._Monster[Monster_cnt[ServerNumber]] = &Monsters[ServerNumber][Monster_cnt[ServerNumber]];
			cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
		}

		auto func0 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 1.7f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 0.85f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		auto func1 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 1.f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 1.f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", func1);
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK2, "Attack", func0);
		//Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::PROJECTILE_ATTACK, "Attack", func1);

		Monster_cnt[ServerNumber]++;
	}
}
void GameManager::InitializeFireMonsters()
{
	{
		Monster ms{ S_ENEMY_TYPE::FIRE_SMALL };
		ms._pos = ms._spawn_pos = Vec3(23.3f, 1.8f, 43.8f);
		ms._look_dir = ms._spawn_dir = Vec3(0.f, 0.f, 1.f);
		ms._hp = ms._max_hp = 50.f;
		ms.LocalTransform();
		for (auto& cl : clients[ServerNumber]) {
			ms._Player[cl.first] = &cl.second._player;
		}
		Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second._player._Monster[Monster_cnt[ServerNumber]] = &Monsters[ServerNumber][Monster_cnt[ServerNumber]];
			cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
		}

		auto func0 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 0.8f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 1.1f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		auto func1 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 2.f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 0.75f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", func1);
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK2, "Attack", func0);
		//Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::PROJECTILE_ATTACK, "Attack", func1);

		Monster_cnt[ServerNumber]++;
	}

	{
		Monster ms{ S_ENEMY_TYPE::FIRE_BIG };
		ms._pos = ms._spawn_pos = Vec3(27.8f, 1.7f, 43.9f);
		ms._look_dir = ms._spawn_dir = Vec3(0.f, 0.f, 1.f);
		ms._hp = ms._max_hp = 100.f;
		ms.LocalTransform();
		for (auto& cl : clients[ServerNumber]) {
			ms._Player[cl.first] = &cl.second._player;
		}
		Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second._player._Monster[Monster_cnt[ServerNumber]] = &Monsters[ServerNumber][Monster_cnt[ServerNumber]];
			cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
		}

		auto func0 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.8f, 1.15f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 1.5f)) {
					cl.second._player.TakeDamage(150);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		auto func1 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 2.3f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 1.f)) {
					cl.second._player.TakeDamage(150);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", func0);
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK2, "Attack", func1);
		//Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::PROJECTILE_ATTACK, "Attack", func1);

		Monster_cnt[ServerNumber]++;
	}

	{
		Monster ms{ S_ENEMY_TYPE::FIRE_SMALL };
		ms._pos = ms._spawn_pos = Vec3(32.6f, 2.1f, 43.65f);
		ms._look_dir = ms._spawn_dir = Vec3(0.f, 0.f, 1.f);
		ms._hp = ms._max_hp = 50.f;
		ms.LocalTransform();
		for (auto& cl : clients[ServerNumber]) {
			ms._Player[cl.first] = &cl.second._player;
		}
		Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second._player._Monster[Monster_cnt[ServerNumber]] = &Monsters[ServerNumber][Monster_cnt[ServerNumber]];
			cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
		}

		auto func0 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 0.8f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 1.1f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		auto func1 = [this](class Monster* monster) {
			Vec2 center = monster->GetWorldOffsetPosition(0.f, 2.f);
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._player._state == S_PLAYER_STATE::JUMP ||
					cl.second._player._state == S_PLAYER_STATE::GATHERING ||
					cl.second._player._state == S_PLAYER_STATE::GETHIT ||
					cl.second._player._state == S_PLAYER_STATE::DEATH ||
					cl.second._player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos = Vec2(cl.second._player._pos.x, cl.second._player._pos.z);
				if (Vec2::IsInRadius(player_pos, center, 0.75f)) {
					cl.second._player.TakeDamage(100);
					cl.second.send_hp_packet((S_OBJECT_TYPE)S_PLAYER, cl.first, cl.second._player._hp, cl.second._player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
			};
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", func1);
		Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::ATTACK2, "Attack", func0);
		//Monsters[ServerNumber][Monster_cnt[ServerNumber]].AddAnimationEvent(S_MONSTER_STATE::PROJECTILE_ATTACK, "Attack", func1);

		Monster_cnt[ServerNumber]++;
	}
}
