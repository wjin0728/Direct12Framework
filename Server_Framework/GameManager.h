#pragma once
#include "stdafx.h"
#include "SESSION.h"
#include "Object.h"
#include <chrono>
	//#include "OVER_PLUS.h"

class GameManager
{
public:
	SOCKET server_socket, client_socket;
	HANDLE h_iocp;
	OVER_PLUS accept_over;
	array<SESSION, MAX_USER> clients;
	Object terrain; // ���� ������

private:
	std::chrono::high_resolution_clock::time_point lastTime;

public:

	GameManager();
	~GameManager();

	void S_Bind_Listen();
	void S_Accept();
	void Make_threads();
	void Worker_thread();
	void Disconnect(int cl_id);
	void Process_packet(int c_id, char* packet);
	int Get_new_Client_id();

	float GetDeltaTime() {
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastTime).count();
		lastTime = currentTime;
		return duration / 1'000'000.0f; // �� ����
	}

	bool CanMove(float x, float z); // ���� �˻�
};
