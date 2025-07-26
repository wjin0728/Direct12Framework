#include "BossMonsterState.h"
#include "GameManager.h"

// BossMonsterState::IdleState 구현         =========================================================================



BossMonsterState::IdleState& BossMonsterState::IdleState::GetInstance() { static BossMonsterState::IdleState instance; return instance; }

void BossMonsterState::IdleState::Enter(Monster* monster) {
	cout << "IdleState Entered!" << endl;
	monster->SetVelocity(0, 0, 0); // 속도 0
	idleTimer = 2.f;
}

void BossMonsterState::IdleState::Update(Monster* monster) {
	idleTimer -= TICK_INTERVAL; // 대기 시간 감소
	if (idleTimer <= 0) {
		monster->SetRandomTarget();
		if (monster->_target) {
			monster->SetState(&BossMonsterState::TargetingState::GetInstance());
		}
	}
}

void BossMonsterState::IdleState::Exit(Monster* monster) {}



// BossMonsterState::TargetingState 구현         =========================================================================



BossMonsterState::TargetingState& BossMonsterState::TargetingState::GetInstance() { static BossMonsterState::TargetingState instance; return instance; }

void BossMonsterState::TargetingState::Enter(Monster* monster) {
	cout << "TargetingState Entered!" << endl;
	monster->SetVelocity(0, 0, 0); // 속도 0
	targetingTimer = 3.f;
	targetingDelay = 1.f;
	sendTarget = false;
	sendTargetLock = false;
	monster->_attack_pos = Vec3::Zero; // 공격 위치 초기화
}

void BossMonsterState::TargetingState::Update(Monster* monster) {
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
			monster->SetState(S_MONSTER_STATE::ATTACK);
		}
	}
}

void BossMonsterState::TargetingState::Exit(Monster* monster) {
	monster->ResetTarget(); // 타겟 초기화
}



// BossMonsterState::AttackState 구현  =========================================================================



BossMonsterState::AttackState& BossMonsterState::AttackState::GetInstance() { static BossMonsterState::AttackState instance; return instance; }

void BossMonsterState::AttackState::Enter(Monster* monster) {
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

void BossMonsterState::AttackState::Update(Monster* monster) {
	attackTimer -= TICK_INTERVAL;
	if (attackTimer <= 0) {
		monster->SetState(S_MONSTER_STATE::IDLE);
	}
}

void BossMonsterState::AttackState::Exit(Monster* monster) {}



// BossMonsterState::SkillState 구현  =========================================================================



BossMonsterState::CastingState& BossMonsterState::CastingState::GetInstance() { static BossMonsterState::CastingState instance; return instance; }

void BossMonsterState::CastingState::Enter(Monster* monster) {
	skillTimer = 2.0f; // 스킬 지속 시간 지정해주기
}

void BossMonsterState::CastingState::Update(Monster* monster) {
	skillTimer -= TICK_INTERVAL;

	switch (rand() % 3)
	{
	default:
		break;
	}
}

void BossMonsterState::CastingState::Exit(Monster* monster) {}



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
	monster->_remove = true;
	monster->_pos.y = -5.f;
	return;
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