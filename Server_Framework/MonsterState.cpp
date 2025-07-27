#include "MonsterState.h"
#include "Monster.h"
#include "GameManager.h"

// MonsterState::IdleState 구현         =========================================================================



MonsterState::IdleState& MonsterState::IdleState::GetInstance() { static MonsterState::IdleState instance; return instance; }

void MonsterState::IdleState::Enter(Monster* monster) {
	//cout << "IdleState Entered!" << endl;
    monster->SetVelocity(0, 0, 0); // 속도 0
	idleTimer = 2.f;
}

void MonsterState::IdleState::Update(Monster* monster) {
	if (monster->_active) idleTimer -= TICK_INTERVAL; // 대기 시간 감소
	if (idleTimer <= 0) {
		if (monster->_target) {
			monster->SetState(S_MONSTER_STATE::RUN);
		}
	}
}

void MonsterState::IdleState::Exit(Monster* monster) {}



// MonsterState::RunState 구현          =========================================================================



MonsterState::RunState& MonsterState::RunState::GetInstance() { static MonsterState::RunState instance; return instance; }

void MonsterState::RunState::Enter(Monster* monster) {
	//cout << "RunState Entered!" << endl;
    // 이동 시작
}

void MonsterState::RunState::Update(Monster* monster) {
	Vec3 pos = monster->_pos; // 현재 위치 저장
	monster->_pos += monster->_velocity * TICK_INTERVAL * (int)(!monster->_on_CantMove);

	if (monster->_target == nullptr) {
		monster->SetState(S_MONSTER_STATE::IDLE);
	}
	if (monster->IsPlayerTooMuchClose()) {
        monster->SetState(S_MONSTER_STATE::ATTACK);
	}
}

void MonsterState::RunState::Exit(Monster* monster) {}



// MonsterState::AttackState 구현  =========================================================================



MonsterState::AttackState& MonsterState::AttackState::GetInstance() { static MonsterState::AttackState instance; return instance; }

void MonsterState::AttackState::Enter(Monster* monster) {
	//cout << "BasicAttackState Entered!" << endl;

	if (rand() % 2) {
		attackTimer = monster->_animations[(int)S_MONSTER_STATE::ATTACK].mLength;
	}
	else {
        attackTimer = monster->_animations[(int)S_MONSTER_STATE::ATTACK2].mLength;
		monster->_state = S_MONSTER_STATE::ATTACK2;
	}
}

void MonsterState::AttackState::Update(Monster* monster) {
    attackTimer -= TICK_INTERVAL;
	if (attackTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::IDLE);
	}
}

void MonsterState::AttackState::Exit(Monster* monster) {}



// MonsterState::SkillState 구현  =========================================================================



MonsterState::SkillState& MonsterState::SkillState::GetInstance() { static MonsterState::SkillState instance; return instance; }

void MonsterState::SkillState::Enter(Monster* monster) {
    skillTimer = 2.0f; // 스킬 지속 시간 지정해주기
}

void MonsterState::SkillState::Update(Monster* monster) {
    skillTimer -= TICK_INTERVAL;
    if (skillTimer <= 0) {
        monster->SetState(S_MONSTER_STATE::IDLE);
    }
}

void MonsterState::SkillState::Exit(Monster* monster) {}



// MonsterState::HitState 구현          =========================================================================



MonsterState::HitState& MonsterState::HitState::GetInstance() { static MonsterState::HitState instance; return instance; }

void MonsterState::HitState::Enter(Monster* monster) {
	//cout << "HitState Entered!" << endl;
    monster->SetVelocity(0, 0, 0); // 이동 멈춤
	hitTimer = monster->_animations[(int)S_MONSTER_STATE::GETHIT].mLength;
}

void MonsterState::HitState::Update(Monster* monster) {
	hitTimer -= TICK_INTERVAL; // 히트 애니메이션 시간 감소
	if (hitTimer <= 0) {
		monster->SetState(monster->previousState); // 이전 상태로 돌아가기
	}
}

void MonsterState::HitState::Exit(Monster* monster) {
}



// MonsterState::DeathState 구현          =========================================================================



MonsterState::DeathState& MonsterState::DeathState::GetInstance() { static MonsterState::DeathState instance; return instance; }

void MonsterState::DeathState::Enter(Monster* monster) {
	//cout << "DeathState Entered!" << endl;
	monster->SetVelocity(0, 0, 0);
	monster->_drop_item = false;
	deathTimer = monster->_animations[(int)S_MONSTER_STATE::DEATH].mLength;
}

void MonsterState::DeathState::Update(Monster* monster) {
	deathTimer -= TICK_INTERVAL;
	if (deathTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::UNDERGROUND);
	}
}

void MonsterState::DeathState::Exit(Monster* monster) {}



// MonsterState::UndergroundState 구현          =========================================================================



MonsterState::UndergroundState& MonsterState::UndergroundState::GetInstance() { static MonsterState::UndergroundState instance; return instance; }

void MonsterState::UndergroundState::Enter(Monster* monster) {
	//cout << "UndergroundState Entered!" << endl;
	monster->SetVelocity(0, 0, 0);
	if (monster->_drop_item) {
		monster->_remove = true;
		monster->_pos.y = -5.f;
		return;
	}
}

void MonsterState::UndergroundState::Update(Monster* monster) {
}

void MonsterState::UndergroundState::Exit(Monster* monster) {
}



// MonsterState::SpawnState 구현          =========================================================================



MonsterState::SpawnState& MonsterState::SpawnState::GetInstance() { static MonsterState::SpawnState instance; return instance; }

void MonsterState::SpawnState::Enter(Monster* monster) {
	cout << "SpawnState Entered!" << endl;
	monster->SetVelocity(0, 0, 0);
	SpawnTimer = monster->_animations[(int)S_MONSTER_STATE::SPAWN].mLength;
}

void MonsterState::SpawnState::Update(Monster* monster) {
	SpawnTimer -= TICK_INTERVAL; 
	if (SpawnTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::IDLE); 
	}
}

void MonsterState::SpawnState::Exit(Monster* monster) {}


////////////////////////////////////////////////// 보스 상태 머신 ///////////////////////////////////////////////////////

// MonsterState::BossIdleState 구현         =========================================================================



MonsterState::BossIdleState& MonsterState::BossIdleState::GetInstance() { static MonsterState::BossIdleState instance; return instance; }

void MonsterState::BossIdleState::Enter(Monster* monster) {
	cout << "IdleState Entered!" << endl;
	monster->SetVelocity(0, 0, 0); // 속도 0
	idleTimer = 2.f;
}

void MonsterState::BossIdleState::Update(Monster* monster) {
	idleTimer -= TICK_INTERVAL; // 대기 시간 감소
	if (idleTimer <= 0) {
		monster->SetRandomTarget();
		if (monster->_target) {
			monster->SetState(&MonsterState::BossTargetingState::GetInstance());
		}
	}
}

void MonsterState::BossIdleState::Exit(Monster* monster) {}



// MonsterState::BossTargetingState 구현         =========================================================================



MonsterState::BossTargetingState& MonsterState::BossTargetingState::GetInstance() { static MonsterState::BossTargetingState instance; return instance; }

void MonsterState::BossTargetingState::Enter(Monster* monster) {
	cout << "TargetingState Entered!" << endl;
	monster->SetVelocity(0, 0, 0); // 속도 0
	targetingTimer = 3.f;
	targetingDelay = 0.75f;
	sendTarget = false;
	sendTargetLock = false;
	monster->_attack_pos = Vec3::Zero; // 공격 위치 초기화
}

void MonsterState::BossTargetingState::Update(Monster* monster) {
	if (targetingTimer > 0) {
		monster->UpdateTarget();
		targetingTimer -= TICK_INTERVAL;
		if (targetingTimer <= 0) {
			monster->_attack_pos = monster->_target->_pos; // 타겟팅 위치 저장
		}
	}
	else {
		targetingDelay -= TICK_INTERVAL; // 타겟팅 딜레이 감소
		if (targetingDelay <= 0) {
			monster->SetState(S_MONSTER_STATE::SKILL);
			//if (pattern_cnt == 2) {
			//	monster->SetState(S_MONSTER_STATE::SKILL);
			//	pattern_cnt = 0;
			//}
			//else {
			//	monster->SetState(S_MONSTER_STATE::ATTACK);
			//}
		}
	}
}

void MonsterState::BossTargetingState::Exit(Monster* monster) {
	pattern_cnt++;
}



// MonsterState::BossAttackState 구현  =========================================================================



MonsterState::BossAttackState& MonsterState::BossAttackState::GetInstance() { static MonsterState::BossAttackState instance; return instance; }

void MonsterState::BossAttackState::Enter(Monster* monster) {
	//cout << "BasicAttackState Entered!" << endl;

	attackTimer = monster->_animations[(int)S_MONSTER_STATE::ATTACK].mLength;
	//if (rand() % 2 == 0) {
	//	attackTimer = monster->_animations[(int)S_MONSTER_STATE::ATTACK].mLength;
	//}
	//else {
	//	attackTimer = monster->_animations[(int)S_MONSTER_STATE::ATTACK2].mLength;
	//	monster->_state = S_MONSTER_STATE::ATTACK2;
	//}
}

void MonsterState::BossAttackState::Update(Monster* monster) {
	attackTimer -= TICK_INTERVAL;
	if (attackTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::IDLE);
	}
}

void MonsterState::BossAttackState::Exit(Monster* monster) {
}



// MonsterState::BossSkillState 구현  =========================================================================



MonsterState::BossSkillState& MonsterState::BossSkillState::GetInstance() { static MonsterState::BossSkillState instance; return instance; }

void MonsterState::BossSkillState::Enter(Monster* monster) {
	skillTimer = monster->_animations[(int)S_MONSTER_STATE::SKILL].mLength;

	switch (rand() % 3)
	{
	case 0: { // 덩쿨 
		Vec3 pos = monster->_attack_pos;
		BoundingBox box(pos, Vec3(4.46f, 1.2f, 4.46f) / 2.f);
		box.Center.y += 0.3f;
		for (auto& player : monster->_Player) {
			if (!player) continue;
			if (player->_state == S_PLAYER_STATE::JUMP ||
				player->_state == S_PLAYER_STATE::GATHERING ||
				player->_state == S_PLAYER_STATE::GETHIT ||
				player->_state == S_PLAYER_STATE::DEATH ||
				player->_state == S_PLAYER_STATE::ULTIMATE)
				continue;

			player->LocalTransform();
			if (box.Intersects(player->_boundingbox)) {
				player->TakeDamage(M_GRASS_VINE_DAMAGE);
				player->cant_move_time = 4.f; // 2초간 이동 불가
				player->_on_CantMove = true;
				SetSkillType(S_GRASS_VINE);
				hit_client_id.emplace_back(player->_id); // 히트된 클라이언트 ID 저장
			}
		}
		break;
	}
	case 1: { // 폭발
		Vec3 pos = monster->_attack_pos;
		pos.y += 1.5f;
		BoundingSphere sphere(pos, 0.7f);
		for (auto& player : monster->_Player) {
			if (!player) continue;
			if (player->_state == S_PLAYER_STATE::JUMP ||
				player->_state == S_PLAYER_STATE::GATHERING ||
				player->_state == S_PLAYER_STATE::GETHIT ||
				player->_state == S_PLAYER_STATE::DEATH ||
				player->_state == S_PLAYER_STATE::ULTIMATE)
				continue;

			player->LocalTransform();
			if (sphere.Intersects(player->_boundingbox)) {
				player->TakeDamage(M_FIRE_EXPLOSION_DAMAGE);
				SetSkillType(S_FIRE_EXPLOSION);
				hit_client_id.emplace_back(player->_id); // 히트된 클라이언트 ID 저장
			}
		}
		break;
	}
	case 2: { // 힐
		monster->_hp += 30; // 보스 HP 증가
		SetSkillType(S_WATER_HEAL);
		break;
	}
	default:
		break;
	}

	sendSkill = true;
}

void MonsterState::BossSkillState::Update(Monster* monster) {
	skillTimer -= TICK_INTERVAL;
	if (skillTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::IDLE);
	}
}

void MonsterState::BossSkillState::Exit(Monster* monster) {
}



// MonsterState::BossHitState 구현          =========================================================================



MonsterState::BossHitState& MonsterState::BossHitState::GetInstance() { static MonsterState::BossHitState instance; return instance; }

void MonsterState::BossHitState::Enter(Monster* monster) {
	//cout << "HitState Entered!" << endl;
	monster->SetVelocity(0, 0, 0); // 이동 멈춤
	hitTimer = monster->_animations[(int)S_MONSTER_STATE::GETHIT].mLength;
}

void MonsterState::BossHitState::Update(Monster* monster) {
	hitTimer -= TICK_INTERVAL; // 히트 애니메이션 시간 감소
	if (hitTimer <= 0) {
		monster->SetState(monster->previousState); // 이전 상태로 돌아가기
	}
}

void MonsterState::BossHitState::Exit(Monster* monster) {
}



// MonsterState::BossDeathState 구현          =========================================================================



MonsterState::BossDeathState& MonsterState::BossDeathState::GetInstance() { static MonsterState::BossDeathState instance; return instance; }

void MonsterState::BossDeathState::Enter(Monster* monster) {
	//cout << "DeathState Entered!" << endl;
	monster->SetVelocity(0, 0, 0);
	deathTimer = monster->_animations[(int)S_MONSTER_STATE::DEATH].mLength;
}

void MonsterState::BossDeathState::Update(Monster* monster) {
	deathTimer -= TICK_INTERVAL;
	if (deathTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::UNDERGROUND);
	}
}

void MonsterState::BossDeathState::Exit(Monster* monster) {}



// MonsterState::BossUndergroundState 구현          =========================================================================



MonsterState::BossUndergroundState& MonsterState::BossUndergroundState::GetInstance() { static MonsterState::BossUndergroundState instance; return instance; }

void MonsterState::BossUndergroundState::Enter(Monster* monster) {
	//cout << "UndergroundState Entered!" << endl;
	monster->SetVelocity(0, 0, 0);
	monster->_remove = true;
	monster->_pos.y = -5.f;
	return;
}

void MonsterState::BossUndergroundState::Update(Monster* monster) {
}

void MonsterState::BossUndergroundState::Exit(Monster* monster) {
}



// MonsterState::BossSpawnState 구현          =========================================================================



MonsterState::BossSpawnState& MonsterState::BossSpawnState::GetInstance() { static MonsterState::BossSpawnState instance; return instance; }

void MonsterState::BossSpawnState::Enter(Monster* monster) {
	cout << "SpawnState Entered!" << endl;
	monster->SetVelocity(0, 0, 0);
	SpawnTimer = monster->_animations[(int)S_MONSTER_STATE::SPAWN].mLength;
}

void MonsterState::BossSpawnState::Update(Monster* monster) {
	SpawnTimer -= TICK_INTERVAL;
	if (SpawnTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::IDLE);
	}
}

void MonsterState::BossSpawnState::Exit(Monster* monster) {}