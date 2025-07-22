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
			clients[ServerNumber][c_id]._player._class = S_PLAYER_CLASS::ARCHER;
		}
		else if (1 == c_id) {
			clients[ServerNumber][c_id]._player._class = S_PLAYER_CLASS::FIGHTER;
		}
		else if (2 == c_id)
			clients[ServerNumber][c_id]._player._class = S_PLAYER_CLASS::MAGE;

		clients[ServerNumber][c_id]._player._pos = Vec3(4.803865f, 0.4409764f, 8.894886f);
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
	}
}
