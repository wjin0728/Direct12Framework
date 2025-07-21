#pragma once
#include "Lobbystdafx.h"
#include "SESSION.h"
#include <chrono>

class GameManager
{
public:
	SOCKET server_socket, client_socket;
	HANDLE h_iocp;
	OVER_PLUS accept_over;
	std::vector<std::thread> workerThreads; 

	array<unordered_map<int, SESSION>, 6> clients;

	int ServerNumber = 0; // 임시로 쓸 서버 번호

	GameManager();
	~GameManager();

	void S_Bind_Listen();
	void S_Accept();
	void StartWorkerThreads(); 
	void Worker_thread();
	void Disconnect(int cl_id);
	void Process_packet(int c_id, char* packet);

	static GameManager& GetInstance() {
		static GameManager instance;
		return instance;
	}

	void Run() {
		while (true) {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	int Get_new_Client_id();

private:

};
