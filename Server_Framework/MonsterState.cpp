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
	idleTimer -= TICK_INTERVAL; // 대기 시간 감소
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
	monster->_pos += monster->_velocity * TICK_INTERVAL;
	//if (monster->gameManager.CanMove(pos.x, pos.z)) {
	//	monster->_pos = pos; // 이동 가능하면 위치 업데이트
	//}
	//else {
	//	monster->SetVelocity(0, 0, 0); // 이동 불가능하면 속도 0
	//}


	if (monster->IsPlayerTooMuchClose()) {
        monster->SetState(S_MONSTER_STATE::ATTACK);
   //     switch (rand() % pattern_cnt)
   //     {
   //     case 0: {
   //         monster->SetState(S_MONSTER_STATE::ATTACK);
			//break;
   //     }
   //     case 1: {
			//monster->SetState(S_MONSTER_STATE::SKILL);
			//break;
   //     }
   //     case 2: {
			//monster->SetState(S_MONSTER_STATE::SKILL); // 원거리?
			//break;
   //     }
   //     default:
   //         break;
   //     }
	}
}

void MonsterState::RunState::Exit(Monster* monster) {}



// MonsterState::AttackState 구현  =========================================================================



MonsterState::AttackState& MonsterState::AttackState::GetInstance() { static MonsterState::AttackState instance; return instance; }

void MonsterState::AttackState::Enter(Monster* monster) {
	//cout << "BasicAttackState Entered!" << endl;

	if (rand() % 2 == 0) {
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
		--monster->_wave;
		monster->SetState(S_MONSTER_STATE::UNDERGROUND);
	}
}

void MonsterState::DeathState::Exit(Monster* monster) {}



// MonsterState::UndergroundState 구현          =========================================================================



MonsterState::UndergroundState& MonsterState::UndergroundState::GetInstance() { static MonsterState::UndergroundState instance; return instance; }

void MonsterState::UndergroundState::Enter(Monster* monster) {
	//cout << "UndergroundState Entered!" << endl;
	monster->_pos = monster->_spawn_pos;
	if (!monster->_wave) {
		monster->_remove = true;
		return;
	}
	monster->_look_dir = monster->_spawn_dir;
	monster->_target = nullptr;
	monster->_hp = monster->_max_hp;
	monster->SetVelocity(0, 0, 0);
	monster->LocalTransform();
}

void MonsterState::UndergroundState::Update(Monster* monster) {
	if (monster->_target != nullptr) {
		monster->SetState(S_MONSTER_STATE::SPAWN);
	}
}

void MonsterState::UndergroundState::Exit(Monster* monster) {
	monster->_pos.y = 5.f;
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