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

	array<Terrain, (int)S_SCENE_TYPE::END> terrain;
	S_SCENE_TYPE scene_type = S_SCENE_TYPE::LOBBY; // 현재 씬 타입

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
	void ChangeScene(uint8_t scene) {
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second.send_change_scene_packet(scene);
		}

		// 씬 전환 후, 몬스터, 아이템 초기화
		Monsters[ServerNumber].clear();
		items[ServerNumber].clear();
		Projectiles[ServerNumber].clear();
		Monster_cnt[ServerNumber] = 0;
		Item_cnt[ServerNumber] = 0;
		Projectile_cnt[ServerNumber] = 0;

		for (auto& cl : clients[ServerNumber]) {
			switch ((S_SCENE_TYPE)scene)
			{
			case S_SCENE_TYPE::LOBBY: {
				cl.second._player._pos = Vec3(0.f, 0.f, 0.f); // 로비 초기 위치
				break;
			}
			case S_SCENE_TYPE::MAINSTAGE1: {
				cl.second._player._pos = Vec3(45.2f, 4.2f, 42.f); // 메인 스테이지 1 초기 위치
				break;
			}
			case S_SCENE_TYPE::MAINSTAGE2: {
				cl.second._player._pos = Vec3(50.2f, 4.6f, 40.f); // 메인 스테이지 2 초기 위치
				break;
			}
			default:
				break;
			}
			cl.second._player._pos = Vec3(0.f, 0.f, 0.f); // 이걸로 초기 위치 잡기
			cl.second._player._velocity = Vec3::Zero;
			cl.second._player._hp = cl.second._player.PlayerMaxHp();
			cl.second._player._barrier = 0;
			cl.second._player.SetState((UINT8)S_PLAYER_STATE::IDLE);
		}
	}

private:
	void Update();

	void LogTickDelay(long long microseconds) {
		printf("Tick %llu delayed: %lld us\n", current_tick, microseconds);
	}
};
