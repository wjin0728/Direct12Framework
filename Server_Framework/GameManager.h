#pragma once
#include "stdafx.h"
#include "SESSION.h"
#include "Terrain.h"
#include "Object.h"
#include "Item.h"
#include "Monster.h"
#include "Projectile.h"
#include <chrono>

	//#include "OVER_PLUS.h"

class GameManager
{
public:
	SOCKET server_socket, client_socket;
	HANDLE h_iocp;
	OVER_PLUS accept_over;
	std::vector<std::thread> workerThreads; 

	const std::chrono::microseconds TICK_DURATION{ 16667 }; // 60Hz
	uint64_t current_tick = 0;

	Terrain terrain;

	array<unordered_map<int, Item>, 6> items;
	array<unordered_map<int, Monster>, 6> Monsters; 
	array<unordered_map<int, SESSION>, 6> clients;
	array<unordered_map<int, Projectile>, 6> Projectiles; // 각 서버에 연결된 클라이언트 ID 집합

	int ServerNumber = 0; // 임시로 쓸 서버 번호

	array<int, 6> Item_cnt = { 0, 0, 0, 0, 0, 0 };
	array<int, 6> Monster_cnt = { 0, 0, 0, 0, 0, 0 };
	array<int, 6> Projectile_cnt = { 0, 0, 0, 0, 0, 0 };

	GameManager();
	~GameManager();

	void S_Bind_Listen();
	void S_Accept();
	void StartWorkerThreads(); 
	void Worker_thread();
	void Disconnect(int cl_id);
	void Process_packet(int c_id, char* packet);

	void SendAllPlayersPosPacket();
	void SendAllMonstersPosPacket();
	void SendAllItemsPosPacket();
	void SendAllProjectilesPosPacket();

	static GameManager& GetInstance() {
		static GameManager instance;
		return instance;
	}

	void Run() {
		auto lastTickTime = std::chrono::steady_clock::now();
		while (true) {
			auto now = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTickTime);

			if (elapsed >= TICK_DURATION) {
				Update();
				current_tick++;
				lastTickTime += TICK_DURATION;
				if (elapsed > TICK_DURATION * 2) {
					lastTickTime = now;
					LogTickDelay(elapsed.count());
				}
			}

			std::this_thread::sleep_for(std::chrono::microseconds(1000));
		}
	}

	int Get_new_Client_id();

	bool CanMove(float x, float z); // 지형 검사

private:
	void Update();

	void LogTickDelay(long long microseconds) {
		printf("Tick %llu delayed: %lld us\n", current_tick, microseconds);
	}
};
