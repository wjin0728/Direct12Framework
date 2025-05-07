#include "GameManager.h"

GameManager::GameManager()
{
	lastTime = std::chrono::high_resolution_clock::now();
	//terrain.SetScale(45, 20, 45);
	terrain.SetScale(64, 598.9f, 64);
	terrain.SetResolution(513);
	terrain.SetNavMapResolution(terrain.GetResolution() * 2);
	//terrain.LoadHeightMap("LobbyTerrainHeightmap");
	//terrain.LoadNavMap("LobbyTerrainNavMap");
	terrain.LoadHeightMap("Battle1TerrainHeightmap");
	terrain.LoadNavMap("Battle1TerrainNavMask");
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

	Make_threads();
}
GameManager::~GameManager()
{
	closesocket(server_socket);
	WSACleanup();
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
	std::cout << "Bind successful.\n";

	if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed: " << WSAGetLastError() << "\n";
		closesocket(server_socket);
		WSACleanup();
		exit(1);
	}
	std::cout << "Listening on port " << PORT_NUM << "\n";
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

void GameManager::Make_threads()
{
	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(&GameManager::Worker_thread, this);

	for (auto& th : worker_threads)
		th.join();
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
					lock_guard<mutex> ll(clients[client_id]._s_lock);
					clients[client_id]._state = ST_ALLOC;
				}
				clients[client_id]._id = client_id;
				clients[client_id]._prev_remain = 0;
				clients[client_id]._socket = client_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket), h_iocp, client_id, 0);
				clients[client_id].do_recv();
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
			int remain_data = num_bytes + clients[key]._prev_remain;
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
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
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
	closesocket(clients[c_id]._socket);

	lock_guard<mutex> ll(clients[c_id]._s_lock);
	clients[c_id]._state = ST_FREE;
}

int GameManager::Get_new_Client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		lock_guard <mutex> ll{ clients[i]._s_lock };
		if (clients[i]._state == ST_FREE)
			return i;
	}
	return -1;
}

void GameManager::Process_packet(int c_id, char* packet)
{
	switch (packet[2]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet); {
			lock_guard<mutex> ll{ clients[c_id]._s_lock };
			clients[c_id]._state = ST_INGAME;
		}

		//clients[c_id]._player._class = p->name[0];
		if (0 == c_id) 
			clients[c_id]._player.SetClass(S_PLAYER_CLASS::FIGHTER);
		else if (1 == c_id) 
			clients[c_id]._player.SetClass(S_PLAYER_CLASS::ARCHER);
		else if (2 == c_id) 
			clients[c_id]._player.SetClass(S_PLAYER_CLASS::FIGHTER);

		clients[c_id]._player._pos = Vec3(27, 6, 22);

		clients[c_id].send_login_info_packet();
		cout << "login : " << c_id << endl;

		// 지금 login한 클라이언트 정보 -> 다른 클라이언트에게 전송
		for (auto& cl : clients) {
			if (cl._state != ST_INGAME) continue;
			cl.send_add_player_packet(&clients[c_id]);
		}
		// 다른 클라이언트 정보 -> 지금 login한 클라이언트에게 전송
		for (auto& cl : clients) {
			if (cl._state != ST_INGAME) continue;
			if (cl._id == c_id) continue;
			clients[c_id].send_add_player_packet(&cl);
		}
		break;
	}
	case CS_CHAT: {
		CS_CHAT_PACKET* p = reinterpret_cast<CS_CHAT_PACKET*>(packet);

		for (auto& cl : clients) {
			if (cl._state != ST_INGAME) continue;
			cl.send_chat_packet(c_id, p->mess);
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

		const float moveSpeed = 5.0f; 
		Vec3 velocity = Vec3::Zero;

		if (moveDir.LengthSquared() > 0.0001f) {
			moveDir.Normalize();
			velocity = moveDir * moveSpeed;
		}
		else {
			velocity = Vec3::Zero;
			clients[c_id]._player._acceleration = Vec3::Zero;
		}

		// 위치 업데이트 60프레임 기준.
		float deltaTime = 1.f / 60.f; 
		Vec3 newPos = clients[c_id]._player._pos + velocity * deltaTime;

		if (CanMove(newPos.x, newPos.z)) {
			clients[c_id]._player._pos = newPos;

			float terrainHeight = terrain.GetHeight(clients[c_id]._player._pos.x, clients[c_id]._player._pos.z);
			clients[c_id]._player._pos.y = terrainHeight;

			if (moveDir.LengthSquared() > 0.001f) {
				clients[c_id]._player._look_dir = moveDir;
			}

			clients[c_id]._player._velocity = velocity;

			float rotationSpeed = 10.f;
			if (moveDir.LengthSquared() > 0.001f) {
				Quaternion targetRot = Quaternion::LookRotation(moveDir);
				Quaternion rotation = clients[c_id]._player._rotation = Quaternion::Slerp(clients[c_id]._player._rotation, targetRot, rotationSpeed * deltaTime);
				Vec3 angle = Vec3::GetAngleToQuaternion(rotation);
				clients[c_id]._player._look_dir.y = angle.y * radToDeg;
			}

			// 플레이어 - 아이템 충돌 체크
			{
				clients[c_id]._player.LocalTransform();
				if (!items.empty()) {
					for (auto& it : items) {
						if (it.second._item_type > S_ITEM_TYPE::S_GRASS_WEAKEN)
							it.second.LocalTransform();
						if (clients[c_id]._player._boundingbox.Intersects(it.second._boundingbox)) {
							for (auto& cl : clients) {
								if (cl._state != ST_INGAME) continue;
								cl.send_remove_item_packet(it.first, c_id, it.second._item_type);
							}
							cout << "cl : " << c_id << "랑 item : " << it.first << " 충돌~!!!!!!!!!!!!!!!" << endl;
							items.erase(it.first);
							break;
						}
					}
				}
			}

			for (auto& cl : clients) {
				if (cl._state != ST_INGAME) continue;
				cl.send_move_packet(&clients[c_id]);
			}
		}
		else {
			clients[c_id]._player._velocity = Vec3::Zero;
			clients[c_id]._player._acceleration = Vec3::Zero;
		}

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
			clients[c_id]._player._on_FireEnchant = true;
		}
		else if (S_WATER_HEAL == p->skill_enum) {
			for (auto& cl : clients)
				cl._player._hp = min((cl._player._hp + WATER_HEAL_AMT), cl._player.PlayerMaxHp());
		}
		else if (S_WATER_SHIELD == p->skill_enum) {
			for (auto& cl : clients)
				cl._player._barrier = 2;
		}
		else if (S_GRASS_WEAKEN == p->skill_enum) {
			clients[c_id]._player._on_GrassWeaken = true;
		}
		break;
	}
	case CS_ULTIMATE_SKILL: {
		CS_ULTIMATE_SKILL_PACKET* p = reinterpret_cast<CS_ULTIMATE_SKILL_PACKET*>(packet);
		break;
	}
	case CS_000: {
		CS_000_PACKET* p = reinterpret_cast<CS_000_PACKET*>(packet);

		items[item_cnt].SetPosition(clients[c_id]._player._pos.x + 3, 
			clients[c_id]._player._pos.y + 0.3, clients[c_id]._player._pos.z);
		items[item_cnt].SetItemType(S_ITEM_TYPE::S_FIRE_ENCHANT);
		items[item_cnt].LocalTransform();

		for (auto& cl : clients) {
			if (cl._state != ST_INGAME) continue;
			cl.send_drop_item_packet(items[item_cnt], item_cnt);
		}

		item_cnt++;
		break;
	}
	}
}

bool GameManager::CanMove(float x, float z)
{
	if (terrain.mNavMapData.empty()) {
		return false;
	}
	float localX = x - terrain.GetOffset().x;
	float localZ = z - terrain.GetOffset().z;
	if (localX < 0.0f || localZ < 0.0f || localX >= terrain.GetScale().x || localZ >= terrain.GetScale().z) {
		return false;
	}
	float xIndex = localX / (terrain.GetScale().x / (terrain.GetNavMapResolution()));
	float zIndex = localZ / (terrain.GetScale().z / (terrain.GetNavMapResolution()));
	zIndex = terrain.GetNavMapResolution() - zIndex;
	xIndex = static_cast<int>(xIndex);
	zIndex = static_cast<int>(zIndex);

	int idx = xIndex + (zIndex * terrain.GetNavMapResolution());
	if (terrain.mNavMapData[idx] != 0) {
		return true;
	}

	return false;
}