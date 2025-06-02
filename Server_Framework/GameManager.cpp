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
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet); {
			lock_guard<mutex> ll{ clients[ServerNumber][c_id]._s_lock };
			clients[ServerNumber][c_id]._state = ST_INGAME;
		}

		if (0 == c_id) {
			clients[ServerNumber][c_id]._player._pos = Vec3(45.2, 4.2, 42);
			clients[ServerNumber][c_id]._player._class = S_PLAYER_CLASS::ARCHER;
		}
		else if (1 == c_id) {
			clients[ServerNumber][c_id]._player._pos = Vec3(50.2, 4.6, 40);
			clients[ServerNumber][c_id]._player._class = S_PLAYER_CLASS::FIGHTER;
		}
		else if (2 == c_id)
			clients[ServerNumber][c_id]._player._class = S_PLAYER_CLASS::MAGE;

		clients[ServerNumber][c_id].send_login_info_packet();
		cout << "login : " << c_id << endl;

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
			cout << "Send add player " << c_id << " 에게 " << cl.first << endl;
		}

		clients[ServerNumber][c_id]._player.SetState((UINT8)S_PLAYER_STATE::IDLE);
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
			clients[ServerNumber][c_id]._player._velocity = moveDir * 2.f;
		}
		else {
			clients[ServerNumber][c_id]._player._velocity = Vec3::Zero;
		}

		break;
	}
	case CS_MOUSE_LDOWN: {
		CS_MOUSE_LDOWN_PACKET* p = reinterpret_cast<CS_MOUSE_LDOWN_PACKET*>(packet);
		Vec3 local_lookDir = Vec3(p->dir_x, p->dir_y, p->dir_z);
		
		switch (clients[ServerNumber][p->id]._player._class)
		{
		case S_PLAYER_CLASS::FIGHTER: {
			for (auto& mon : Monsters[ServerNumber]) {
				mon.second.LocalTransform();
				clients[ServerNumber][c_id]._player.OnFighterBasicAttack(mon.second._boundingbox);
			}
			break;
		}
		case S_PLAYER_CLASS::ARCHER: {
			Projectile proj{1, S_PROJECTILE_TYPE::ARROW};
			proj._pos = clients[ServerNumber][c_id]._player._pos;
			proj._pos.y += 1.f; // 발사 위치 조정
			proj._velocity = local_lookDir;
			Projectiles[ServerNumber].insert({ Projectile_cnt[ServerNumber], proj });
			for (auto& cl : clients[ServerNumber]) {
				cl.second.send_add_projectile_packet(proj, Projectile_cnt[ServerNumber]);
			}
			Projectile_cnt[ServerNumber]++;
			break;
		}
		case S_PLAYER_CLASS::MAGE: {
			Projectile proj{ 1, S_PROJECTILE_TYPE::MAGIC_BALL };
			proj._pos = clients[ServerNumber][c_id]._player._pos;
			proj._velocity = local_lookDir;
			Projectiles[ServerNumber].insert({ Projectile_cnt[ServerNumber], proj });
			for (auto& cl : clients[ServerNumber]) {
				clients[ServerNumber][c_id].send_add_projectile_packet(proj, Projectile_cnt[ServerNumber]);
			}
			Projectile_cnt[ServerNumber]++;
			break;
		}
		default:
			break;
		}
		Quaternion targetRot = Quaternion::LookRotation(local_lookDir);
		Vec3 angle = Vec3::GetAngleToQuaternion(targetRot) * radToDeg;
		clients[ServerNumber][p->id]._player._rotation = targetRot;
		clients[ServerNumber][p->id]._player.SetLookDir(angle);
		break;
	}
	case CS_SKILL_TARGET: {
		CS_SKILL_TARGET_PACKET* p = reinterpret_cast<CS_SKILL_TARGET_PACKET*>(packet);

		if (S_FIRE_EXPLOSION == p->skill_enum) {}
		else if (S_GRASS_VINE == p->skill_enum) {}
		break;
	}
	case CS_SKILL_NONTARGET: {
		CS_SKILL_NONTARGET_PACKET* p = reinterpret_cast<CS_SKILL_NONTARGET_PACKET*>(packet);

		if (S_FIRE_ENCHANT == p->skill_enum) {
			clients[ServerNumber][c_id]._player._on_FireEnchant = true;
		}
		else if (S_WATER_HEAL == p->skill_enum) {
			for (auto& cl : clients[ServerNumber])
				cl.second._player._hp = min((cl.second._player._hp + WATER_HEAL_AMT), cl.second._player.PlayerMaxHp());
		}
		else if (S_WATER_SHIELD == p->skill_enum) {
			for (auto& cl : clients[ServerNumber])
				cl.second._player._barrier = 2;
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
			{
				Monster ms{ S_ENEMY_TYPE::GRASS_SMALL };
				ms._pos = Vec3(50.f, 5.f, 50.f);
				ms._look_dir = Vec3(0.f, 0.f, 1.f);
				ms.LocalTransform();
				for (auto& cl : clients[ServerNumber]) {
					ms._Player[cl.first] = &cl.second._player;
				}
				Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
				for (auto& cl : clients[ServerNumber]) {
					if (cl.second._state != ST_INGAME) continue;
					cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
				}
				Monster_cnt[ServerNumber]++;
			}

			{
				Monster ms{ S_ENEMY_TYPE::GRASS_BIG };
				ms._pos = Vec3(55.f, 5.f, 50.f);
				ms._look_dir = Vec3(0.f, 0.f, 1.f);
				ms.LocalTransform();	
				for (auto& cl : clients[ServerNumber]) {
					ms._Player[cl.first] = &cl.second._player;
				}
				Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;			
				for (auto& cl : clients[ServerNumber]) {
					if (cl.second._state != ST_INGAME) continue;
					cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
				}
				Monster_cnt[ServerNumber]++;
			}

			{
				Monster ms{ S_ENEMY_TYPE::GRASS_SMALL };
				ms._pos = Vec3(60.f, 5.f, 50.f);
				ms._look_dir = Vec3(0.f, 0.f, 1.f);
				ms.LocalTransform();
				for (auto& cl : clients[ServerNumber]) {
					ms._Player[cl.first] = &cl.second._player;
				}
				Monsters[ServerNumber][Monster_cnt[ServerNumber]] = ms;
				for (auto& cl : clients[ServerNumber]) {
					if (cl.second._state != ST_INGAME) continue;
					cl.second.send_add_monster_packet(Monsters[ServerNumber][Monster_cnt[ServerNumber]], Monster_cnt[ServerNumber]);
				}
				Monster_cnt[ServerNumber]++;
			}
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

void GameManager::Update() {
	for (auto& cl : clients[ServerNumber]) {
		if (cl.second._state != ST_INGAME) continue;
		cl.second._player.Update();

		if (cl.second._player.HasMoveInput()) {
			Vec3 newPos = cl.second._player._pos + (cl.second._player._velocity * TICK_INTERVAL);

			if (CanMove(newPos.x, newPos.z)) {
				cl.second._player._pos = newPos;

				float terrainHeight = terrain[(int)scene_type].GetHeight(cl.second._player._pos.x, cl.second._player._pos.z);
				cl.second._player._pos.y = terrainHeight;

				float rotationSpeed = 10.f;
				if (cl.second._player._velocity.LengthSquared() > 0.001f) {
					Quaternion targetRot = Quaternion::LookRotation(cl.second._player._velocity);
					Quaternion rotation = cl.second._player._rotation = Quaternion::Slerp(cl.second._player._rotation, targetRot, rotationSpeed * TICK_INTERVAL);
					Vec3 angle = Vec3::GetAngleToQuaternion(rotation);
					cl.second._player._look_dir.y = angle.y * radToDeg;
				}

				// 플레이어 - 아이템 충돌 체크
				{
					if (!items.empty()) {
						for (auto& it : items[ServerNumber]) {
							if (it.second._item_type > S_ITEM_TYPE::S_GRASS_WEAKEN)
								it.second.LocalTransform();
							if (cl.second._player._boundingbox.Intersects(it.second._boundingbox)) {
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
				}
			}
			else {
				cl.second._player._velocity = Vec3::Zero;
			}
		}
	}
	for (auto& proj : Projectiles[ServerNumber]) {
		proj.second.Update();
		if (abs(proj.second._pos.x) > 100.f || abs(proj.second._pos.z) > 100.f) {
			//for (auto& cl : clients[ServerNumber]) {
			//	if (cl.second._state != ST_INGAME) continue;
			//	cl.second.send_remove_projectile_packet(proj.first, cl.first);
			//}
			Projectiles[ServerNumber].erase(proj.first);
			continue;
		}
	}
	for (auto& ms : Monsters[ServerNumber]) {
		ms.second.Update();
		if (ms.second._hp >= 0) {
			for (auto& proj : Projectiles[ServerNumber]) {
				if (!proj.second._user_frinedly) continue; // 적이 쏜 projectile면 패스
				if (ms.second._boundingbox.Intersects(proj.second._boundingbox)) {
					ms.second.SetState((UINT8)S_MONSTER_STATE::DEATH);
					//cout << "몬스터 " << ms.first << "가 projectile " << proj.first << "에 맞았습니다." << endl;
				}
			}
		}
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
