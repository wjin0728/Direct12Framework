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
struct MonsterWave {
	int current_wave = -1;

	float wave_timer = WAVE_INTERVAL;
	float message_timer = 0.f; // 메시지 타이머
	int message_count = 0; // 메시지 카운트

	bool is_end = true; // 웨이브 종료 여부
	bool is_spawn = false; // 스폰 여부
	bool make_potal = false; // 포탈 생성 여부

	void Initialize(S_SCENE_TYPE scene_type) {
		if (scene_type == S_SCENE_TYPE::LOBBY) // 로비만 Intro 사용
			current_wave = -1;
		else
			current_wave = 0;

		wave_timer = WAVE_INTERVAL;
		is_spawn = false;
		is_end = true;
		make_potal = false;
	}
};

struct MonsterAttackInfo {
	Vec2 offset;
	float radius;
	int damage;
};

class GameManager
{
public:
	struct SpawnData {
		Vec3 pos;
		Quaternion rot;
	};

	SOCKET server_socket, client_socket;
	HANDLE h_iocp;
	OVER_PLUS accept_over;
	std::vector<std::thread> workerThreads; 

	const std::chrono::microseconds TICK_DURATION{ 16667 }; // 60Hz
	uint64_t current_tick = 0;

	array<Terrain, (int)S_SCENE_TYPE::END> terrain;
	array<array<SpawnData, (int)PLAYER_CLASS::end>, (int)S_SCENE_TYPE::END> spawnDatas; // 각 씬의 스폰 포인트
	S_SCENE_TYPE scene_type = S_SCENE_TYPE::LOBBY; // 현재 씬 타입

	array<unordered_map<int, Item>, 6> items;
	array<unordered_map<int, Monster>, 6> Monsters; 
	array<unordered_map<int, SESSION>, 6> clients;
	array<unordered_map<int, Projectile>, 6> Projectiles; // 각 서버에 연결된 클라이언트 ID 집합

	int ServerNumber = 0; // 임시로 쓸 서버 번호

	array<int, 6> Item_cnt = { 0, 0, 0, 0, 0, 0 };
	array<int, 6> Monster_cnt = { 0, 0, 0, 0, 0, 0 };
	array<int, 6> Projectile_cnt = { 0, 0, 0, 0, 0, 0 };
	array<MonsterWave, 6> MonsterWaves; // 각 서버의 몬스터 웨이브 정보

	const float boss_item_spawn_interval = 15.f; // 보스 아이템 생성 간격 (초 단위)
	float boss_item_timer = boss_item_spawn_interval; // 보스 아이템 생성 타이머

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
	void SendHPPacket(S_OBJECT_TYPE type, int id, int hp, int shield);
	void SendMakePortalPacket(); // 포탈 생성
	void SendMakeMessagePacket(uint8_t wave_type);
	void SendAddProjectilePacket(Projectile& proj, int proj_id);

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
		if (scene >= (uint8_t)S_SCENE_TYPE::END) return; // 유효하지 않은 씬 타입 체크
		if (scene_type == (S_SCENE_TYPE)scene) 
			return; // 현재 씬과 동일한 씬으로 전환 시도 방지
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			cl.second.send_change_scene_packet(scene);
		}

		scene_type = (S_SCENE_TYPE)scene; // 씬 타입 업데이트

		// 씬 전환 후, 몬스터, 아이템 초기화
		Monsters[ServerNumber].clear();
		items[ServerNumber].clear();
		Projectiles[ServerNumber].clear();

		Monster_cnt[ServerNumber] = 0;
		Item_cnt[ServerNumber] = 0;
		Projectile_cnt[ServerNumber] = 0;

		MonsterWaves[ServerNumber].Initialize(scene_type); // 웨이브 상태 초기화

		for (auto& cl : clients[ServerNumber]) {
			cl.second._player._pos = cl.second._player._spawn_pos = spawnDatas[(int)scene_type][(int)cl.second._player._class].pos;
			cl.second._player._rotation = cl.second._player._spawn_rotation = spawnDatas[(int)scene_type][(int)cl.second._player._class].rot;
			cl.second._player._velocity = Vec3::Zero;
			cl.second._player._hp = cl.second._player.PlayerMaxHp();
			cl.second._player._barrier = 0;
			cl.second._player.SetState((UINT8)S_PLAYER_STATE::IDLE);
			cl.second._player.InitializeTarget();
			cl.second._player._is_revival = true;
			//cl.second._player._ready_for_next_stage = false;
		}

		cout << "Scene changed to: " << (int)scene_type << endl;
	}
	void ChangeScene() {
		switch (scene_type) {
		case S_SCENE_TYPE::LOBBY:
			ChangeScene((uint8_t)S_SCENE_TYPE::MAIN_STAGE_1);
			break;
		case S_SCENE_TYPE::MAIN_STAGE_1:
			ChangeScene((uint8_t)S_SCENE_TYPE::MAIN_STAGE_2);
			break;
		case S_SCENE_TYPE::MAIN_STAGE_2:
			ChangeScene((uint8_t)S_SCENE_TYPE::MAIN_STAGE_3);
			break;
		case S_SCENE_TYPE::MAIN_STAGE_3:
			ChangeScene((uint8_t)S_SCENE_TYPE::ENDING);
			break;
		}
	}

	void CreateItem(S_ENEMY_TYPE a, float x, float z);
	void CreateItemAtRandomPosition();

	void InitializeWave();
	void InitializeMonster(S_ENEMY_TYPE type, Vec3 position);

	std::function<void(Monster*)> MakeAttackEvent(Vec2 offset, float radius, int damage);

	void UpdateWave();
	void HandleWaveEnd(MonsterWave& wave);
	void HandleWaveInProgress(MonsterWave& wave);
	//bool IsAllPlayerReady(); // 모든 플레이어가 다음 스테이지 준비 상태인지 확인

private:
	void Update();

	void LogTickDelay(long long microseconds) {
		printf("Tick %llu delayed: %lld us\n", current_tick, microseconds);
	}

	bool IsClassOK(S_PLAYER_CLASS class_type) const {
		for (auto& cl : clients[ServerNumber]) {
			if (cl.second._state != ST_INGAME) continue;
			if (cl.second._player._class == class_type) return false;
		}
		return true;
	};
	bool IsAllClassSelected() const {
		for (int i = 0; i < (int)S_PLAYER_CLASS::end; ++i) {
			if (IsClassOK((S_PLAYER_CLASS)i))
				return false;
		}
		return true;
	}

	std::map<S_ENEMY_TYPE, std::vector<MonsterAttackInfo>> attackInfos = {
	{ S_ENEMY_TYPE::GRASS_SMALL, {
		{ {0.2f, 0.9f}, 2.f, M_SMALL_DAMAGE },
		{ {0.f, 0.7f}, 0.8f, M_SMALL_DAMAGE },
	}},
	{ S_ENEMY_TYPE::GRASS_BIG, {
		{ {0.f, 2.85f}, 1.f, M_BIG_DAMAGE },
		{ {1.25f, 1.25f}, 2.f, M_BIG_DAMAGE },
	}},
	{ S_ENEMY_TYPE::WATER_SMALL, {
		{ {0.f, 1.f}, 1.f, M_SMALL_DAMAGE },
		{ {0.f, 1.7f}, 0.85f, M_SMALL_DAMAGE },
	}},
	{ S_ENEMY_TYPE::WATER_BIG, {
		{ {0.f, 2.5f}, 1.9f, M_BIG_DAMAGE },
		{ {0.f, 0.f}, 2.5f, M_BIG_DAMAGE },
	}},
	{ S_ENEMY_TYPE::FIRE_SMALL, {
		{ {0.f, 2.f}, 0.75f, M_SMALL_DAMAGE },
		{ {0.f, 0.8f}, 1.1f, M_SMALL_DAMAGE },
	}},
	{ S_ENEMY_TYPE::FIRE_BIG, {
		{ {0.8f, 1.15f}, 1.5f, M_BIG_DAMAGE },
		{ {0.f, 2.3f}, 1.f, M_BIG_DAMAGE },
	}}
	};
};
