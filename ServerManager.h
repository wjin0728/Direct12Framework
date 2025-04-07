#pragma once

#include "../Serverframework/TriumServer/protocol.h"
#include "Player.h"
#include <mutex>

class ServerManager
{
	MAKE_SINGLETON(ServerManager)
public:
	unordered_map <int, Player>			players;
	SOCKET								send_socket, server_soket;
	WSAOVERLAPPED						wsaover;

	char								SERVER_ADDR[NAME_SIZE]{ "127.0.0.1" };

	Player								my_info;

	size_t save_data_size = 0;
	size_t one_packet_size = 0;
	char save_buf[CHAT_SIZE * 2];

	mutex object_lock;

	void Client_Login();

	void Recv_Packet();
	static void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag);
	void Recv_Loop();
	void Send_Packet(void* packet);
	static void CALLBACK send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag);
	void Using_Packet(char* packet_ptr);
	void print_error(const char* msg, int err_no);

	void send_cs_move_packet() {
		CS_MOVE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.id = my_info.id;

		Send_Packet(&p);
	}
};

