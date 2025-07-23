#include "GameManager.h"

GameManager::GameManager()
{
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
	server_addr.sin_port = htons(LOBBY_PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	std::cout << "Binding to port " << LOBBY_PORT_NUM << "\n";
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
			clients[client_id].send_login_info_packet();
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
		//CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet); {
		//	lock_guard<mutex> ll{ clients[c_id]._s_lock };
		//	clients[c_id]._state = ST_INGAME;
		//}
		break;
	}
	case CS_CLICK_BUTTON: {
		CS_CLICK_BUTTON_PACKET* p = reinterpret_cast<CS_CLICK_BUTTON_PACKET*>(packet);

		switch ((S_BUTTON_TYPE)p->button_type)
		{
		case S_BUTTON_TYPE::MAIN_UI_GAME_START: {
			clients[c_id].send_room_player_count_packet(Room_Cnt);


			clients[c_id].send_lobby_server_out_packet();
			break;
		}
		case S_BUTTON_TYPE::ROOM1:
		case S_BUTTON_TYPE::ROOM2:
		case S_BUTTON_TYPE::ROOM3:
		case S_BUTTON_TYPE::ROOM4:
		case S_BUTTON_TYPE::ROOM5:
		case S_BUTTON_TYPE::ROOM6: {
			if (Room_Cnt[(int)p->button_type] >= 3) { return; }

			Room_Cnt[(int)S_BUTTON_TYPE::ROOM1]++;

			for (auto& client : clients) {
				if (client.second._state == ST_INGAME) {
					client.second.send_room_player_count_packet(Room_Cnt);
				}
			}

			break;
		}
		default:
			break;
		}
		break;
	}
	}
}
