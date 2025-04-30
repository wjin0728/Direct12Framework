#pragma once
#include "stdafx.h"
#include "SESSION.h"
#include "Terrain.h"
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
	array<Object, MAX_ITEM> items;
	int item_cnt = 0; // 아이템 id
	Terrain terrain; // 지형 데이터

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
		return duration / 1'000'000.0f; // 초 단위
	}

	bool CanMove(float x, float z); // 지형 검사
};
