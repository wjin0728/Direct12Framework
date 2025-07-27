#include "GameManager.h"

GameManager::GameManager()
{
	terrain[(int)S_SCENE_TYPE::LOBBY].SetScale(45, 20, 45);
	terrain[(int)S_SCENE_TYPE::LOBBY].SetResolution(513);
	terrain[(int)S_SCENE_TYPE::LOBBY].SetNavMapResolution(terrain[(int)S_SCENE_TYPE::LOBBY].GetResolution() * 2);
	terrain[(int)S_SCENE_TYPE::LOBBY].LoadHeightMap("LobbyTerrainHeightmap");
	terrain[(int)S_SCENE_TYPE::LOBBY].LoadNavMap("LobbyTerrainNavMask");
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_1].SetScale(100.f, 598.9f, 100.f);
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_1].SetResolution(513);
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_1].SetNavMapResolution(terrain[(int)S_SCENE_TYPE::MAIN_STAGE_1].GetResolution() * 2);
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_1].LoadHeightMap("Battle1TerrainHeightmap");
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_1].LoadNavMap("Battle1TerrainNavMask");
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_2].SetScale(64.f, 600.9f, 64.f);
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_2].SetResolution(513);
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_2].SetNavMapResolution(terrain[(int)S_SCENE_TYPE::MAIN_STAGE_1].GetResolution() * 2);
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_2].LoadHeightMap("Battle2TerrainHeightmap");
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_2].LoadNavMap("Battle2TerrainNavMask");
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_3].SetScale(64.f, 600.f, 64.f);
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_3].SetResolution(513);
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_3].SetNavMapResolution(terrain[(int)S_SCENE_TYPE::MAIN_STAGE_1].GetResolution() * 2);
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_3].LoadHeightMap("Battle3TerrainHeightmap");
	terrain[(int)S_SCENE_TYPE::MAIN_STAGE_3].LoadNavMap("Battle3TerrainNavMask");
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

	std::ifstream lobbySpawnData("..\\Resources\\Scenes\\LobbySpawnData.bin", std::ios::binary);
	if (!lobbySpawnData) {
		std::cerr << "Failed to open lobby spawn data file.\n";
	}
	std::string token;
	while (1) {
		BinaryReader::ReadDateFromFile(lobbySpawnData, token);
		if (token == "<Archer>") {
			Vec3 spawnPoint;
			Quaternion spawnRot;
			BinaryReader::ReadDateFromFile(lobbySpawnData, spawnPoint);
			BinaryReader::ReadDateFromFile(lobbySpawnData, spawnRot);
			spawnDatas[(int)S_SCENE_TYPE::LOBBY][(int)S_PLAYER_CLASS::ARCHER].pos = spawnPoint;
			spawnDatas[(int)S_SCENE_TYPE::LOBBY][(int)S_PLAYER_CLASS::ARCHER].rot = spawnRot;
		}
		else if (token == "<Fighter>") {
			Vec3 spawnPoint;
			Quaternion spawnRot;
			BinaryReader::ReadDateFromFile(lobbySpawnData, spawnPoint);
			BinaryReader::ReadDateFromFile(lobbySpawnData, spawnRot);
			spawnDatas[(int)S_SCENE_TYPE::LOBBY][(int)S_PLAYER_CLASS::FIGHTER].pos = spawnPoint;
			spawnDatas[(int)S_SCENE_TYPE::LOBBY][(int)S_PLAYER_CLASS::FIGHTER].rot = spawnRot;
		}
		else if (token == "<Mage>") {
			Vec3 spawnPoint;
			Quaternion spawnRot;
			BinaryReader::ReadDateFromFile(lobbySpawnData, spawnPoint);
			BinaryReader::ReadDateFromFile(lobbySpawnData, spawnRot);
			spawnDatas[(int)S_SCENE_TYPE::LOBBY][(int)S_PLAYER_CLASS::MAGE].pos = spawnPoint;
			spawnDatas[(int)S_SCENE_TYPE::LOBBY][(int)S_PLAYER_CLASS::MAGE].rot = spawnRot;
		}
		if (token == "</SpawnData>") 
			break;
	}

	for (int i = (int)S_SCENE_TYPE::MAIN_STAGE_1; i < (int)S_SCENE_TYPE::ENDING; i++) {
		std::ifstream battleSpawnData("..\\Resources\\Scenes\\Battle" + std::to_string(i - (int)S_SCENE_TYPE::MAIN_STAGE_1 + 1) + "SpawnData.bin", std::ios::binary);
		std::string token;
		while (1) {
			BinaryReader::ReadDateFromFile(battleSpawnData, token);
			if (token == "<Archer>") {
				Vec3 spawnPoint;
				Quaternion spawnRot;
				BinaryReader::ReadDateFromFile(battleSpawnData, spawnPoint);
				BinaryReader::ReadDateFromFile(battleSpawnData, spawnRot);
				spawnDatas[i][(int)S_PLAYER_CLASS::ARCHER].pos = spawnPoint;
				spawnDatas[i][(int)S_PLAYER_CLASS::ARCHER].rot = spawnRot;
			}
			else if (token == "<Fighter>") {
				Vec3 spawnPoint;
				Quaternion spawnRot;
				BinaryReader::ReadDateFromFile(battleSpawnData, spawnPoint);
				BinaryReader::ReadDateFromFile(battleSpawnData, spawnRot);
				spawnDatas[i][(int)S_PLAYER_CLASS::FIGHTER].pos = spawnPoint;
				spawnDatas[i][(int)S_PLAYER_CLASS::FIGHTER].rot = spawnRot;
			}
			else if (token == "<Mage>") {
				Vec3 spawnPoint;
				Quaternion spawnRot;
				BinaryReader::ReadDateFromFile(battleSpawnData, spawnPoint);
				BinaryReader::ReadDateFromFile(battleSpawnData, spawnRot);
				spawnDatas[i][(int)S_PLAYER_CLASS::MAGE].pos = spawnPoint;
				spawnDatas[i][(int)S_PLAYER_CLASS::MAGE].rot = spawnRot;
			}
			if (token == "</SpawnData>") 
				break;
		}
	}

	cout << "Spawn Point OK" << endl;
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
				clients[ServerNumber][client_id].send_login_info_packet();
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

		cout << "login : " << c_id << endl;

		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			if (cl.first == c_id) continue;
			if (cl.second._player._class == S_PLAYER_CLASS::end) continue; // 클래스가 선택되지 않은 클라이언트는 제외
			clients[ServerNumber][c_id].send_add_player_packet(&cl.second);
			cout << "클라 " << cl.first << "의 정보 " << c_id << "에게 전송 완료" << endl;
		}
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
		Vec3 pos = Monsters[ServerNumber][p->target_id]._pos;
		
		if (S_FIRE_EXPLOSION == p->skill_enum) {
			pos.y += 1.5f;
			BoundingSphere sphere(pos, 0.7f);
			for (auto& mon : Monsters[ServerNumber]) {
				if (mon.second.IsUnavailable()) continue;
			
				mon.second.LocalTransform();
				if (sphere.Intersects(mon.second._boundingbox)) {
					mon.second.TakeDamage(P_FIRE_EXPLOSION_DAMAGE, true);
					SendHPPacket((S_OBJECT_TYPE)S_ENEMY, mon.first, mon.second._hp, 0);
				}
			}
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._state != ST_INGAME) continue;
				Vec3 pos = Monsters[ServerNumber][p->target_id]._pos;
				pos.y += 1.5f;
				cl.second.send_add_effect_packet((int)S_EFFECT_TYPE::EXPLOSION, pos);
			}
		}
		else if (S_GRASS_VINE == p->skill_enum) {
			BoundingBox box(pos, Vec3(4.46f, 1.2f, 4.46f)/2.f);
			box.Center.y += 0.3f; 
			for (auto& mon : Monsters[ServerNumber]) {
				if (mon.second.IsUnavailable()) continue;
				mon.second.LocalTransform();
				if (box.Intersects(mon.second._boundingbox)) {
					mon.second.TakeDamage(P_GRASS_VINE_DAMAGE, true);
					mon.second.cant_move_time = 4.f; // 2초간 이동 불가
					mon.second._on_CantMove = true;
					SendHPPacket((S_OBJECT_TYPE)S_ENEMY, mon.first, mon.second._hp, 0);

					std::cout << "Monster " << mon.first << " hit by Grass Vine skill." << std::endl;
				}
			}
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._state != ST_INGAME) continue;
				Vec3 pos = Monsters[ServerNumber][p->target_id]._pos;
				cl.second.send_add_effect_packet((int)S_EFFECT_TYPE::VINE, pos);
			}
		}
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
				SendHPPacket((S_OBJECT_TYPE)S_PLAYER, c_id, cl.second._player._hp, cl.second._player._barrier);
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
		auto& player = clients[ServerNumber][c_id]._player;
		if (!player._target || player._target->IsUnavailable()) break; // 타겟 몬스터가 없는 경우

		switch (player._class)
		{
		case S_PLAYER_CLASS::FIGHTER: {
			player._target->TakeDamage(P_ULTIMAGE_DAMAGE, true); // 전사 궁극기: 타겟 몬스터에게 50의 피해
			break;
		}
		case S_PLAYER_CLASS::ARCHER: {
			cout << "Archer CS_ULTIMATE_SKILL\n";
			Projectile proj{ 1, S_PROJECTILE_TYPE::ULTIMATE_ARROW };
			proj._pos = player._pos;
			proj._pos.y += 2.5f;

			Vec3 target_pos = player._target->_pos + Vec3{ 0.f, 1.f, 0.f };
			Vec3 direction = target_pos - proj._pos;

			direction.Normalize();
			proj._velocity = direction / 3.f;

			Projectiles[ServerNumber].insert({ Projectile_cnt[ServerNumber], proj });
			SendAddProjectilePacket(proj, Projectile_cnt[ServerNumber]);
			Projectile_cnt[ServerNumber]++;
			break;
		}
		case S_PLAYER_CLASS::MAGE: {
			cout << "Mage CS_ULTIMATE_SKILL\n";

			const int num_projectiles = 5;
			const float radius = 1.0f; // 오각형 반지름
			const float height_offset = 1.f;

			// 플레이어 위치 기준으로 시작
			Vec3 center = player._pos;
			center.y += height_offset;

			// 플레이어의 바라보는 방향에 수직인 벡터 계산 (오각형을 회전시키기 위해)
			Vec3 up{ 0.f, 1.f, 0.f };
			Vec3 right = up.Cross(player._look_dir);
			right.Normalize();

			// 오각형 꼭짓점 각도
			float angle_offset = XM_2PI / num_projectiles;

			// 각 투사체 생성
			for (int i = 0; i < num_projectiles; ++i) {
				float angle = angle_offset * i;

				// 로컬 공간에서의 오각형 점 위치
				float local_x = cosf(angle) * radius;
				float local_y = sinf(angle) * radius;

				// 3D 공간에서의 투사체 위치 계산
				Vec3 offset = right * local_x + up * local_y;
				Vec3 spawn_pos = center + offset;

				// 투사체 방향: 오각형 중심(플레이어 앞쪽)으로 날아가게
				Vec3 target_pos = player._target->_pos + Vec3{ 0.f, 1.f, 0.f };
				Vec3 direction = target_pos - spawn_pos;
				direction.Normalize();

				Projectile proj{ 1, S_PROJECTILE_TYPE::MAGIC_BALL };
				proj._pos = spawn_pos;
				proj._velocity = direction / 3.f;

				Projectiles[ServerNumber].insert({ Projectile_cnt[ServerNumber], proj });
				SendAddProjectilePacket(proj, Projectile_cnt[ServerNumber]);
				Projectile_cnt[ServerNumber]++;
				break;
			}
		}
		default:
			break;
		}

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
			InitializeWave();
			break;
		}
		// 씬 전환
		case 2: {
			ChangeScene((uint8_t)S_SCENE_TYPE::LOBBY);
			break;
		}
		// 씬 전환
		case 3: {
			ChangeScene((uint8_t)S_SCENE_TYPE::MAIN_STAGE_1);
			break;
		}
		// 씬 전환
		case 4: {
			ChangeScene((uint8_t)S_SCENE_TYPE::MAIN_STAGE_2);
			break;
		}
		case 5: {
			ChangeScene((uint8_t)S_SCENE_TYPE::MAIN_STAGE_3);
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
			Matrix mLocalMat = Matrix::CreateFromQuaternion(player._rotation);
			mLocalMat._41 = player._pos.x;
			mLocalMat._42 = player._pos.y;
			mLocalMat._43 = player._pos.z;
			BoundingOrientedBox originAtbox;
			BoundingOrientedBox atbox;
			atbox.Extents = Vec3(1.557609f, 1.032651f, 1.580357f) / 2.f;
			atbox.Center = { 0, 0.533f, 0.828f };
			originAtbox.Transform(atbox, mLocalMat);

			for (auto& mon : Monsters[ServerNumber]) {
				if (mon.second.IsUnavailable()) continue; // 몬스터가 제거된 경우는 패스
				mon.second.LocalTransform();
				if (atbox.Intersects(mon.second._boundingbox)) {
					mon.second.TakeDamage(P_FIGHTER_DAMAGE, false);
					SendHPPacket(S_OBJECT_TYPE::S_ENEMY, mon.first, mon.second._hp, 0);
				}
			}
			break;
		}
		case S_PLAYER_CLASS::ARCHER: {
			cout << "Archer CS_ATTACK\n";
			Projectile proj{ 1, S_PROJECTILE_TYPE::ARROW };
			proj._pos = player._pos;
			proj._pos.y += 0.3f;

			if (player._target == nullptr) { // 타겟 몬스터가 제거된 경우
				direction.x = sin(player._look_dir.y * degToRad); // 1.0
				direction.y = 0.0f;
				direction.z = cos(player._look_dir.y * degToRad); // 0.0
			}
			else if (not player._target->IsUnavailable()) { // 타겟 몬스터가 있고 유효한 경우
				Vec3 target_pos = player._target->_pos + Vec3{ 0.f, 1.f, 0.f };
				direction = target_pos - proj._pos;
			}

			direction.Normalize();
			proj._velocity = direction / 3.f;

			Projectiles[ServerNumber].insert({ Projectile_cnt[ServerNumber], proj });
			SendAddProjectilePacket(proj, Projectile_cnt[ServerNumber]);
			Projectile_cnt[ServerNumber]++;
			break;
		}
		case S_PLAYER_CLASS::MAGE: {
			Projectile proj{ 1, S_PROJECTILE_TYPE::MAGIC_BALL };

			proj._pos = player._pos;
			proj._pos.y += 0.3f;

			if (player._target == nullptr) { // 타겟 몬스터가 제거된 경우
				direction.x = sin(player._look_dir.y * degToRad); // 1.0
				direction.y = 0.0f;
				direction.z = cos(player._look_dir.y * degToRad); // 0.0
			}
			else if (not player._target->IsUnavailable()) { // 타겟 몬스터가 있고 유효한 경우
				Vec3 target_pos = player._target->_pos + Vec3{ 0.f, 1.f, 0.f };
				direction = target_pos - proj._pos;
			}

			direction.Normalize();
			proj._velocity = direction / 3.f;

			Projectiles[ServerNumber].insert({ Projectile_cnt[ServerNumber], proj });
			SendAddProjectilePacket(proj, Projectile_cnt[ServerNumber]);
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

			//cout << "Select Class >> " << p->button_type << endl;
			clients[ServerNumber][c_id]._player.SetClass((S_PLAYER_CLASS)(p->button_type));
			clients[ServerNumber][c_id]._player._pos = spawnDatas[(int)S_SCENE_TYPE::LOBBY][(int)clients[ServerNumber][c_id]._player._class].pos;
			clients[ServerNumber][c_id]._player._rotation = spawnDatas[(int)S_SCENE_TYPE::LOBBY][(int)clients[ServerNumber][c_id]._player._class].rot;

			// 지금 login한 클라이언트 정보 -> 다른 클라이언트에게 전송
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._state != ST_INGAME) continue;
				cl.second.send_add_player_packet(&clients[ServerNumber][c_id]);
				//cout << "클라 " << c_id << "의 정보 " << cl.first << "에게 전송 완료" << endl;
			}
			// 다른 클라이언트 정보 -> 지금 login한 클라이언트에게 전송
			for (auto& cl : clients[ServerNumber]) {
				if (cl.second._state != ST_INGAME) continue;
				if (cl.first == c_id) continue;
				if (cl.second._player._class == S_PLAYER_CLASS::end) continue; // 클래스가 선택되지 않은 클라이언트는 제외
				clients[ServerNumber][c_id].send_add_player_packet(&cl.second);
				//cout << "클라 " << cl.first << "의 정보 " << c_id << "에게 전송 완료" << endl;
			}

			if (IsAllClassSelected()) { // 모든 플레이어가 클래스 선택을 완료한 경우
				MonsterWaves[ServerNumber].wave_timer = -1.f; // 초기 웨이브 끝, S_INTRO 웨이브 시작
			}
			break;
		}
		default:
			break;
		}

		clients[ServerNumber][c_id]._player.SetState((UINT8)S_PLAYER_STATE::IDLE);
		break;
	}
	case CS_HP: {
		CS_HP_PACKET* p = reinterpret_cast<CS_HP_PACKET*>(packet);
		Monsters[ServerNumber][p->object_id].TakeDamage(p->hp, false);
		break;
	}
	case CS_READY_FOR_NEXT_STAGE: {
		ChangeScene();
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
	for (auto& cl : clients[ServerNumber]) {
		auto& player = cl.second._player;
		if (cl.second._state != ST_INGAME) continue;
		if (player._state == S_PLAYER_STATE::DEATH) continue;

		player.Update();

		switch (player._state) {
		case S_PLAYER_STATE::ULTIMATE: { // 전사 궁극기 점프 처리
			if (player._class == S_PLAYER_CLASS::FIGHTER) {
				float terrainHeight = terrain[(int)scene_type].GetHeight(player._pos.x, player._pos.z);
				player._pos.y = terrainHeight + player._data;
			}
			break;
		}
		case S_PLAYER_STATE::GATHERING: { // 아이템 획득 처리
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
					//cout << "cl : " << cl.first << "랑 item : " << it.first << " 충돌~!!!!!!!!!!!!!!!" << endl;
					items[ServerNumber].erase(it.first);
					break;
				}
			}
			break;
		}
		}

		if (player.HasMoveInput()) {
			Vec3 newPos = player._pos + (player._velocity * TICK_INTERVAL);

			if (CanMove(newPos.x, newPos.z)) {
				float terrainHeight = terrain[(int)scene_type].GetHeight(newPos.x, newPos.z);
				if (terrainHeight < 2.99 && scene_type == S_SCENE_TYPE::MAIN_STAGE_1) continue;
				newPos.y = terrainHeight;

				player._pos = newPos;

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
		monster.Update();

		// 일반 몬스터
		if (monster._class != S_ENEMY_TYPE::BOSS) {
			// 아이템 생성
			if (monster._state == S_MONSTER_STATE::DEATH && !monster._drop_item) {
				CreateItem(monster._class, monster._pos.x, monster._pos.z);
				monster._drop_item = true;
			}

			// 다른 몬스터와 충돌 회피
			monster.AvoidCollision(Monsters[ServerNumber]);

			// 몬스터 위치 업데이트
			if (monster._state == S_MONSTER_STATE::RUN || monster._state == S_MONSTER_STATE::SPAWN) {
				float terrainHeight = terrain[(int)scene_type].GetHeight(monster._pos.x, monster._pos.z);
				monster._pos.y = terrainHeight;
			}
		}
		// 보스 몬스터
		else {
			// 타겟팅 상태 처리
			auto& targeting_state = MonsterState::BossTargetingState::GetInstance();
			if (monster.currentState == &targeting_state) {

				if (not targeting_state.GetSendTarget()) { // 타겟 패킷이 아직 전송되지 않은 경우
					for (auto& cl : clients[ServerNumber]) {
						cl.second.send_boss_set_target_packet(monster._target->_id);
					}
					targeting_state.SetSendTarget(true);
				}

				if (not targeting_state.GetSendTargetLock() && monster._attack_pos != Vec3::Zero) { // 공격 위치가 확정되었고 패킷을 전송하지 않은 경우
					for (auto& cl : clients[ServerNumber]) {
						cl.second.send_boss_target_lock_packet();
					}
					targeting_state.SetSendTargetLock(true);
				}
			}

			auto& skill_state = MonsterState::BossSkillState::GetInstance();
			if (monster.currentState == &skill_state) {
				if (skill_state.GetSendSkill()) {
					Vec3 pos = monster._target->_pos;
					for (auto& cl : clients[ServerNumber]) {
						for (auto& hitid : skill_state.hit_client_id) {
							SendHPPacket((S_OBJECT_TYPE)S_PLAYER, hitid, clients[ServerNumber][hitid]._player._hp, clients[ServerNumber][hitid]._player._barrier);
						}
						if (skill_state.GetSkillType() == S_GRASS_VINE) {
							cl.second.send_add_effect_packet((int)S_EFFECT_TYPE::VINE, pos);
						}
						else if (skill_state.GetSkillType() == S_FIRE_EXPLOSION) {
							pos.y += 1.5f;
							cl.second.send_add_effect_packet((int)S_EFFECT_TYPE::EXPLOSION, pos);
						}
						else if (skill_state.GetSkillType() == S_WATER_HEAL) {

						}
					}
					skill_state.SetSendSkill(false);
				}
			}
		}

		// 이벤트 처리
		if (monster._state == S_MONSTER_STATE::ATTACK || monster._state == S_MONSTER_STATE::ATTACK2 || monster._state == S_MONSTER_STATE::SKILL) {
			monster.HandleCallback(monster.mEventHandler[(int)monster._state]);
		}

		// 몬스터 - 투사체 충돌 체크
		if (monster._hp >= 0 && not monster.IsUnavailable()) {
			for (auto& proj : Projectiles[ServerNumber]) {
				if (!proj.second._user_frinedly) continue; // 적이 쏜 projectile면 패스
				if (proj.second._remove) continue; // 투사체가 제거된 경우는 패스
				if (monster._boundingbox.Intersects(proj.second._boundingbox)) {
					monster.TakeDamage(proj.second._damage, false);
					SendHPPacket((S_OBJECT_TYPE)S_ENEMY, ms.first, monster._hp, 0);
					proj.second._remove = true; // 투사체 제거
				}
			}
		}
	}

	// 몬스터 웨이브 관리
	if (clients[ServerNumber].size()) UpdateWave();

	// 보스 스테이지 처리
	if (scene_type == S_SCENE_TYPE::MAIN_STAGE_3 && MonsterWaves[ServerNumber].current_wave == S_BOSS) {
		// 아이템 생성
		boss_item_timer -= TICK_INTERVAL;
		if (boss_item_timer <= 0.f) {
			CreateItemAtRandomPosition();
			boss_item_timer = boss_item_spawn_interval; // 다음 아이템 생성 타이머 초기화
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
void GameManager::SendHPPacket(S_OBJECT_TYPE type, int id, int hp, int shield) {
	for (auto& cl : clients[ServerNumber]) {
		if (cl.second._state != ST_INGAME) continue;
		cl.second.send_hp_packet(type, id, hp, shield);
	}
}
void GameManager::SendMakePortalPacket()
{
	for (auto& [_, cl] : clients[ServerNumber]) {
		if (cl._state != ST_INGAME) continue;
		cl.send_make_potal_packet();
		//cout << "Send make portal packet to client " << cl._id << std::endl;
	}
}
void GameManager::SendMakeMessagePacket(uint8_t wave_type)
{
	for (auto& [_, cl] : clients[ServerNumber]) {
		if (cl._state != ST_INGAME) continue;
		cl.send_make_message_packet(wave_type);
		//cout << "Send make massege packet to client " << cl._id << std::endl;
	}
}
void GameManager::SendAddProjectilePacket(Projectile& proj, int proj_id) {
	for (auto& [_, cl] : clients[ServerNumber]) {
		if (cl._state != ST_INGAME) continue;
		cl.send_add_projectile_packet(proj, proj_id);
		//cout << "Send add projectile packet to client " << cl._id << std::endl;
	}
}

void GameManager::CreateItem(S_ENEMY_TYPE monster_type, float x, float z)
{
	float terrainHeight = terrain[(int)scene_type].GetHeight(x, z);

	items[ServerNumber][Item_cnt[ServerNumber]].SetPosition(x, terrainHeight + 0.3, z);
	//items[ServerNumber][Item_cnt[ServerNumber]].SetItemType(S_ITEM_TYPE::S_WATER_SHIELD);
	items[ServerNumber][Item_cnt[ServerNumber]].LocalTransform();

	switch (monster_type)
	{
	case S_ENEMY_TYPE::GRASS_SMALL:
	case S_ENEMY_TYPE::GRASS_BIG:	
		items[ServerNumber][Item_cnt[ServerNumber]].SetItemType(S_ITEM_TYPE::S_GRASS_VINE);
		break;
	case S_ENEMY_TYPE::FIRE_SMALL:
	case S_ENEMY_TYPE::FIRE_BIG:
		items[ServerNumber][Item_cnt[ServerNumber]].SetItemType(S_ITEM_TYPE::S_FIRE_EXPLOSION);
		break;
	case S_ENEMY_TYPE::WATER_SMALL:
	case S_ENEMY_TYPE::WATER_BIG:
		items[ServerNumber][Item_cnt[ServerNumber]].SetItemType(S_ITEM_TYPE::S_WATER_SHIELD);
		break;
	case S_ENEMY_TYPE::BOSS:
		items[ServerNumber][Item_cnt[ServerNumber]].SetItemType(rand() % 2 ? S_ITEM_TYPE::S_FIRE_EXPLOSION : S_ITEM_TYPE::S_WATER_SHIELD);
		break;
	default:
		break;
	}

	for (auto& cl : clients[ServerNumber]) {
		if (cl.second._state != ST_INGAME) continue;
		cl.second.send_drop_item_packet(items[ServerNumber][Item_cnt[ServerNumber]], Item_cnt[ServerNumber]);
	}

	Item_cnt[ServerNumber]++;
}
void GameManager::CreateItemAtRandomPosition()
{
	Vec2 randomPos{};
	while (true) {
		randomPos = terrain[(int)scene_type].GetRandomXZ();
		if (CanMove(randomPos.x, randomPos.y)) break;
	}
	CreateItem(S_ENEMY_TYPE::BOSS, randomPos.x, randomPos.y);
}

void GameManager::InitializeWave()
{
	// 몬스터 초기화
	if (Monsters[ServerNumber].size()) {
		for (auto& cl : clients[ServerNumber]) {
			for (int i = 0; i < Monster_cnt[ServerNumber]; ++i) {
				cl.second.send_remove_monster_packet(i);
			}
		}

		Monsters[ServerNumber].clear();
		Monster_cnt[ServerNumber] = 0;
	}

	auto cur_wave = ++MonsterWaves[ServerNumber].current_wave;

	switch (scene_type) {
	case S_SCENE_TYPE::LOBBY: {
		MonsterWaves[ServerNumber].message_count = 3;
		break;
	}
	case S_SCENE_TYPE::MAIN_STAGE_1: {
		if (cur_wave == S_mm) {
			InitializeMonster(S_ENEMY_TYPE::GRASS_SMALL, Vec3(50.f, -5.f, 50.f));
			InitializeMonster(S_ENEMY_TYPE::GRASS_SMALL, Vec3(55.f, -5.f, 50.f));
		}
		else if (cur_wave == S_mMm) {
			InitializeMonster(S_ENEMY_TYPE::GRASS_SMALL, Vec3(50.f, -5.f, 50.f));
			InitializeMonster(S_ENEMY_TYPE::GRASS_BIG, Vec3(55.5f, -5.f, 50.f));
			InitializeMonster(S_ENEMY_TYPE::GRASS_SMALL, Vec3(60.f, -5.f, 50.f));
		}
		MonsterWaves[ServerNumber].message_count = 1;
		break;
	}
	case S_SCENE_TYPE::MAIN_STAGE_2: {
		if (cur_wave == S_mm) {
			InitializeMonster(S_ENEMY_TYPE::WATER_SMALL, Vec3(29.4f, 0.f, 35.6f));
			InitializeMonster(S_ENEMY_TYPE::WATER_SMALL, Vec3(31.33f, 0.f, 33.24f));
		}
		else if (cur_wave == S_mMm) {
			InitializeMonster(S_ENEMY_TYPE::WATER_SMALL, Vec3(29.4f, 0.f, 35.6f));
			InitializeMonster(S_ENEMY_TYPE::WATER_BIG, Vec3(31.33f, 0.f, 33.24f));
			InitializeMonster(S_ENEMY_TYPE::WATER_SMALL, Vec3(32.3f, 0.f, 29.f));
		}
		MonsterWaves[ServerNumber].message_count = 1;
		break;
	}
	case S_SCENE_TYPE::MAIN_STAGE_3: {
		if (cur_wave == S_mm) {
			InitializeMonster(S_ENEMY_TYPE::FIRE_SMALL, Vec3(23.3f, 0.f, 43.8f));
			InitializeMonster(S_ENEMY_TYPE::FIRE_SMALL, Vec3(27.8f, 0.f, 43.9f));
			MonsterWaves[ServerNumber].message_count = 1;
		}
		else if (cur_wave == S_mMm) {
			InitializeMonster(S_ENEMY_TYPE::FIRE_SMALL, Vec3(23.3f, 0.f, 43.8f));
			InitializeMonster(S_ENEMY_TYPE::FIRE_BIG, Vec3(27.8f, 0.f, 43.9f));
			InitializeMonster(S_ENEMY_TYPE::FIRE_SMALL, Vec3(32.6f, 0.f, 43.65f));
			MonsterWaves[ServerNumber].message_count = 1;
		}
		else if (cur_wave == S_BOSS) {
			InitializeMonster(S_ENEMY_TYPE::BOSS, Vec3(22.27432f, 0.9705162f, 28.85343f));
			MonsterWaves[ServerNumber].message_count = 2;
		}
		else if (cur_wave == S_OUTRO) {
			MonsterWaves[ServerNumber].message_count = 2;
		}
		break;
	}
	}

	SendMakeMessagePacket((uint8_t)cur_wave);

	MonsterWaves[ServerNumber].is_end = false;
	MonsterWaves[ServerNumber].wave_timer = WAVE_INTERVAL;
	MonsterWaves[ServerNumber].spawn_timer = SPAWN_INTERVAL;
	MonsterWaves[ServerNumber].message_timer = MESSAGE_INTERVAL * (float)MonsterWaves[ServerNumber].message_count;
}
void GameManager::InitializeMonster(S_ENEMY_TYPE type, Vec3 position)
{
	Monster ms{ type };
	ms._pos = position;
	ms.LocalTransform();
	for (auto& cl : clients[ServerNumber]) {
		ms._Player[cl.first] = &cl.second._player;
	}

	// 공격 이벤트 등록
	if (type != S_ENEMY_TYPE::BOSS) { // 일반 몬스터인 경우
		const auto& infos = attackInfos[type];
		ms.AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", MakeAttackEvent(infos[0].offset, infos[0].radius, infos[0].damage));
		ms.AddAnimationEvent(S_MONSTER_STATE::ATTACK2, "Attack", MakeAttackEvent(infos[1].offset, infos[1].radius, infos[1].damage));
		//ms.AddAnimationEvent(S_MONSTER_STATE::SKILL, "Attack", MakeAttackEvent(infos[1].offset, infos[1].radius, infos[1].damage));
	}
	else { // 보스 몬스터인 경우
		ms._drop_item = true; // 보스 몬스터는 아이템 드랍 설정
		float radius = 5.f; // 공격 범위

		auto func = [this](Monster* monster) {
			for (auto& [id, cl] : clients[ServerNumber]) {
				auto& player = cl._player;
				if (player._state == S_PLAYER_STATE::JUMP ||
					player._state == S_PLAYER_STATE::GATHERING ||
					player._state == S_PLAYER_STATE::GETHIT ||
					player._state == S_PLAYER_STATE::DEATH ||
					player._state == S_PLAYER_STATE::ULTIMATE)
					continue;

				Vec2 player_pos(player._pos.x, player._pos.z);
				Vec2 attack_pos(monster->_attack_pos.x, monster->_attack_pos.z);
				if (Vec2::IsInRadius(attack_pos, player_pos, 3.f)) {
					player.TakeDamage(M_BOSS_DAMAGE);
					SendHPPacket(S_OBJECT_TYPE::S_PLAYER, id, player._hp, player._barrier);
					std::cout << "맞았다!!!!!!!!" << std::endl;
				}
			}
		};
		ms.AddAnimationEvent(S_MONSTER_STATE::ATTACK, "Attack", func);
	}

	int monster_id = Monster_cnt[ServerNumber];
	Monsters[ServerNumber][monster_id] = ms;

	for (auto& [id, cl] : clients[ServerNumber]) {
		if (cl._state != ST_INGAME) continue;

		cl._player._target = nullptr;
		cl._player._Monster[monster_id] = &Monsters[ServerNumber][monster_id];
		cl.send_add_monster_packet(Monsters[ServerNumber][monster_id], monster_id);
	}

	Monster_cnt[ServerNumber]++;
}
std::function<void(Monster*)> GameManager::MakeAttackEvent(Vec2 offset, float radius, int damage)
{
	return [this, offset, radius, damage](Monster* monster)
	{
		Vec2 center = monster->GetWorldOffsetPosition(offset.x, offset.y);
		for (auto& [id, cl] : clients[ServerNumber]) {
			auto& player = cl._player;
			if (player._state == S_PLAYER_STATE::JUMP ||
				player._state == S_PLAYER_STATE::GATHERING ||
				player._state == S_PLAYER_STATE::GETHIT ||
				player._state == S_PLAYER_STATE::DEATH ||
				player._state == S_PLAYER_STATE::ULTIMATE)
				continue;

			Vec2 player_pos(player._pos.x, player._pos.z);
			if (Vec2::IsInRadius(center, player_pos, radius)) {
				player.TakeDamage(damage);
				SendHPPacket(S_OBJECT_TYPE::S_PLAYER, id, player._hp, player._barrier);
				std::cout << "맞았다!!!!!!!!" << std::endl;
			}
		}
	};
}

void GameManager::UpdateWave()
{
	if (scene_type < S_SCENE_TYPE::LOBBY || scene_type > S_SCENE_TYPE::MAIN_STAGE_3) return;

	auto& wave = MonsterWaves[ServerNumber];

	if (wave.is_end) {
		HandleWaveEnd(wave);
	}
	else {
		HandleWaveInProgress(wave);
	}
}
void GameManager::HandleWaveEnd(MonsterWave& wave)
{
	bool isFinalWave{};

	switch (scene_type) {
		case S_SCENE_TYPE::LOBBY: {
			isFinalWave = (wave.current_wave == S_INTRO);
			break;
		}
		case S_SCENE_TYPE::MAIN_STAGE_1:
		case S_SCENE_TYPE::MAIN_STAGE_2: {
			isFinalWave = (wave.current_wave == S_mMm);
			break;
		}
		case S_SCENE_TYPE::MAIN_STAGE_3: {
			isFinalWave = (wave.current_wave == S_OUTRO);
			break;
		}
	}

	if (isFinalWave) { // 마지막 웨이브가 끝났으면 포탈 생성
		if (!wave.make_potal) {
			SendMakePortalPacket();
			wave.make_potal = true;
		}
		// else if (IsAllPlayerReady()) ChangeScene();
	}
	else { // 아직 마지막 웨이브가 아니면 다음 웨이브로
		if (scene_type != S_SCENE_TYPE::LOBBY) wave.wave_timer -= TICK_INTERVAL;
		if (wave.wave_timer <= 0.f) {
			InitializeWave();
			std::cout << wave.current_wave + 1 << " wave started." << std::endl;
		}
	}
}
void GameManager::HandleWaveInProgress(MonsterWave& wave)
{
	if (wave.message_timer > 0) { // 메시지 타이머 처리
		wave.message_timer -= TICK_INTERVAL;
		if (wave.message_timer <= 0.f && wave.current_wave == S_INTRO || wave.current_wave == S_OUTRO) {
			wave.is_end = true;
		}
		return;
	}
	// 메시지 타이머가 끝났으면
	else if (wave.spawn_timer > 0) { // 스폰 처리
		wave.spawn_timer -= TICK_INTERVAL;
		if (wave.spawn_timer <= 0.f) {
			for (auto& [_, monster] : Monsters[ServerNumber]) {
				monster.SetState(S_MONSTER_STATE::SPAWN);
			}
		}
	}

	// 웨이브 종료 조건 확인
	wave.is_end = true;
	for (auto& [_, monster] : Monsters[ServerNumber]) {
		if (!monster._remove || !monster._drop_item) {
			wave.is_end = false;
			break;
		}
	}
}
//bool GameManager::IsAllPlayerReady()
//{
//	bool all_ready = true;
//	for (auto& cl : clients[ServerNumber]) {
//		if (cl.second._state != ST_INGAME) continue;
//		if (cl.second._player._ready_for_next_stage == false)
//			all_ready = false;
//		break;
//	}
//	return all_ready;
//}
