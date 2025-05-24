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

	int item_cnt = 0; 
	int monster_cnt = 0; 
	int projectile_cnt = 0;

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

	void MonsterTargetSet(int monster_id, int target_id) {
		Monsters[ServerNumber][monster_id]._targetId = target_id;
	}

private:
	void Update() {
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second._player.Update();

			if (cl.second._player.HasMoveInput()) {
				Vec3 newPos = cl.second._player._pos + (cl.second._player._velocity * TICK_INTERVAL);

				if (CanMove(newPos.x, newPos.z)) {
					cl.second._player._pos = newPos;

					float terrainHeight = terrain.GetHeight(cl.second._player._pos.x, cl.second._player._pos.z);
					cl.second._player._pos.y = terrainHeight;

					float rotationSpeed = 10.f;
					if (cl.second._player._velocity.LengthSquared() > 0.001f) {
						Quaternion targetRot = Quaternion::LookRotation(cl.second._player._velocity);
						Quaternion rotation = cl.second._player._rotation = Quaternion::Slerp(cl.second._player._rotation, targetRot, rotationSpeed * TICK_INTERVAL);
						Vec3 angle = Vec3::GetAngleToQuaternion(rotation);
						cl.second._player._look_dir.y = angle.y * radToDeg;
					}

					// 플레이어 - 아이템 충돌 체크
					{
						if (!items.empty()) {
							for (auto& it : items[ServerNumber]) {
								if (it.second._item_type > S_ITEM_TYPE::S_GRASS_WEAKEN)
									it.second.LocalTransform();
								if (cl.second._player._boundingbox.Intersects(it.second._boundingbox)) {
									for (auto& cl : clients[ServerNumber]) {
										if (cl.second._state != ST_INGAME) continue;
										cl.second.send_remove_item_packet(it.first, cl.first, it.second._item_type);
									}
									cout << "cl : " << cl.first << "랑 item : " << it.first << " 충돌~!!!!!!!!!!!!!!!" << endl;
									items[ServerNumber].erase(it.first);
									break;
								}
							}
						}
					}
				}
				else {
					cl.second._player._velocity = Vec3::Zero;
				}
			}
		}
		//for (auto& ms : Monsters) {
		//	ms.second.Update();
		//}
		SendAllPlayersPosPacket();
		//SendAllMonstersPosPacket();
		//SendAllItemsPosPacket();
	}

	void LogTickDelay(long long microseconds) {
		printf("Tick %llu delayed: %lld us\n", current_tick, microseconds);
	}
};
