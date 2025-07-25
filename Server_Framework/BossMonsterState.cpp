#include "BossMonsterState.h"
#include "Monster.h"
#include "GameManager.h"

// BossMonsterState::IdleState 구현         =========================================================================



BossMonsterState::IdleState& BossMonsterState::IdleState::GetInstance() { static BossMonsterState::IdleState instance; return instance; }

void BossMonsterState::IdleState::Enter(Monster* monster) {
	//cout << "IdleState Entered!" << endl;
	monster->SetVelocity(0, 0, 0); // 속도 0
	idleTimer = 2.f;
}

void BossMonsterState::IdleState::Update(Monster* monster) {
	idleTimer -= TICK_INTERVAL; // 대기 시간 감소
	if (idleTimer <= 0) {
		if (monster->_target) {
			monster->SetState(S_MONSTER_STATE::RUN);
		}
	}
}

void BossMonsterState::IdleState::Exit(Monster* monster) {}



// BossMonsterState::RunState 구현          =========================================================================



BossMonsterState::RunState& BossMonsterState::RunState::GetInstance() { static BossMonsterState::RunState instance; return instance; }

void BossMonsterState::RunState::Enter(Monster* monster) {
	//cout << "RunState Entered!" << endl;
	// 이동 시작
}

void BossMonsterState::RunState::Update(Monster* monster) {
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

void BossMonsterState::RunState::Exit(Monster* monster) {}



// BossMonsterState::AttackState 구현  =========================================================================



BossMonsterState::AttackState& BossMonsterState::AttackState::GetInstance() { static BossMonsterState::AttackState instance; return instance; }

void BossMonsterState::AttackState::Enter(Monster* monster) {
	//cout << "BasicAttackState Entered!" << endl;

	if (rand() % 2 == 0) {
		attackTimer = monster->_animations[(int)S_MONSTER_STATE::ATTACK].mLength;
	}
	else {
		attackTimer = monster->_animations[(int)S_MONSTER_STATE::ATTACK2].mLength;
		monster->_state = S_MONSTER_STATE::ATTACK2;
	}
}

void BossMonsterState::AttackState::Update(Monster* monster) {
	attackTimer -= TICK_INTERVAL;
	if (attackTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::IDLE);
	}
}

void BossMonsterState::AttackState::Exit(Monster* monster) {}



// BossMonsterState::SkillState 구현  =========================================================================



BossMonsterState::SkillState& BossMonsterState::SkillState::GetInstance() { static BossMonsterState::SkillState instance; return instance; }

void BossMonsterState::SkillState::Enter(Monster* monster) {
	skillTimer = 2.0f; // 스킬 지속 시간 지정해주기
}

void BossMonsterState::SkillState::Update(Monster* monster) {
	skillTimer -= TICK_INTERVAL;
	if (skillTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::IDLE);
	}
}

void BossMonsterState::SkillState::Exit(Monster* monster) {}



// BossMonsterState::HitState 구현          =========================================================================



BossMonsterState::HitState& BossMonsterState::HitState::GetInstance() { static BossMonsterState::HitState instance; return instance; }

void BossMonsterState::HitState::Enter(Monster* monster) {
	//cout << "HitState Entered!" << endl;
	monster->SetVelocity(0, 0, 0); // 이동 멈춤
	hitTimer = monster->_animations[(int)S_MONSTER_STATE::GETHIT].mLength;
}

void BossMonsterState::HitState::Update(Monster* monster) {
	hitTimer -= TICK_INTERVAL; // 히트 애니메이션 시간 감소
	if (hitTimer <= 0) {
		monster->SetState(monster->previousState); // 이전 상태로 돌아가기
	}
}

void BossMonsterState::HitState::Exit(Monster* monster) {
}



// BossMonsterState::DeathState 구현          =========================================================================



BossMonsterState::DeathState& BossMonsterState::DeathState::GetInstance() { static BossMonsterState::DeathState instance; return instance; }

void BossMonsterState::DeathState::Enter(Monster* monster) {
	//cout << "DeathState Entered!" << endl;
	monster->SetVelocity(0, 0, 0);
	monster->_drop_item = false;
	deathTimer = monster->_animations[(int)S_MONSTER_STATE::DEATH].mLength;
}

void BossMonsterState::DeathState::Update(Monster* monster) {
	deathTimer -= TICK_INTERVAL;
	if (deathTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::UNDERGROUND);
	}
}

void BossMonsterState::DeathState::Exit(Monster* monster) {}



// BossMonsterState::UndergroundState 구현          =========================================================================



BossMonsterState::UndergroundState& BossMonsterState::UndergroundState::GetInstance() { static BossMonsterState::UndergroundState instance; return instance; }

void BossMonsterState::UndergroundState::Enter(Monster* monster) {
	//cout << "UndergroundState Entered!" << endl;
	monster->SetVelocity(0, 0, 0);
	if (monster->_drop_item) {
		monster->_remove = true;
		monster->_pos.y = -5.f;
		return;
	}
}

void BossMonsterState::UndergroundState::Update(Monster* monster) {
}

void BossMonsterState::UndergroundState::Exit(Monster* monster) {
}



// BossMonsterState::SpawnState 구현          =========================================================================



BossMonsterState::SpawnState& BossMonsterState::SpawnState::GetInstance() { static BossMonsterState::SpawnState instance; return instance; }

void BossMonsterState::SpawnState::Enter(Monster* monster) {
	cout << "SpawnState Entered!" << endl;
	monster->SetVelocity(0, 0, 0);
	SpawnTimer = monster->_animations[(int)S_MONSTER_STATE::SPAWN].mLength;
}

void BossMonsterState::SpawnState::Update(Monster* monster) {
	SpawnTimer -= TICK_INTERVAL;
	if (SpawnTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::IDLE);
	}
}

void BossMonsterState::SpawnState::Exit(Monster* monster) {}