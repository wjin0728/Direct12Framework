#pragma once
#include "stdafx.h"
#include "SESSION.h"
#include "Terrain.h"
#include "Object.h"
#include "Item.h"
#include "Monster.h"
#include <chrono>

	//#include "OVER_PLUS.h"

class GameManager
{
public:
	SOCKET server_socket, client_socket;
	HANDLE h_iocp;
	OVER_PLUS accept_over;
	std::vector<std::thread> workerThreads; // 워커 스레드 저장 (신규)

	const std::chrono::microseconds TICK_DURATION{ 16667 }; // 60Hz
	uint64_t current_tick = 0;

	Terrain terrain;

	unordered_map<int, Item> items;
	unordered_map<int, Monster> Monsters; 
	unordered_map<int, SESSION> clients;

	int item_cnt = 0; // 아이템 id
	int monster_cnt = 0; // 몬스터 id

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

private:
	void Update() {
		for (auto& cl : clients) {
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
							for (auto& it : items) {
								if (it.second._item_type > S_ITEM_TYPE::S_GRASS_WEAKEN)
									it.second.LocalTransform();
								if (cl.second._player._boundingbox.Intersects(it.second._boundingbox)) {
									for (auto& cl : clients) {
										if (cl.second._state != ST_INGAME) continue;
										cl.second.send_remove_item_packet(it.first, cl.first, it.second._item_type);
									}
									cout << "cl : " << cl.first << "랑 item : " << it.first << " 충돌~!!!!!!!!!!!!!!!" << endl;
									items.erase(it.first);
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
