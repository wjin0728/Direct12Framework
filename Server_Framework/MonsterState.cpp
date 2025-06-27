#include "MonsterState.h"
#include "Monster.h"

// MonsterState::IdleState 구현         =========================================================================



MonsterState::IdleState& MonsterState::IdleState::GetInstance() { static MonsterState::IdleState instance; return instance; }

void MonsterState::IdleState::Enter(Monster* monster) {
	cout << "IdleState Entered!" << endl;
    monster->SetVelocity(0, 0, 0); // 속도 0
}

void MonsterState::IdleState::Update(Monster* monster) {
    if (monster->_target) {
        monster->SetState(S_MONSTER_STATE::RUN);
    }
}

void MonsterState::IdleState::Exit(Monster* monster) {}



// MonsterState::RunState 구현          =========================================================================



MonsterState::RunState& MonsterState::RunState::GetInstance() { static MonsterState::RunState instance; return instance; }

void MonsterState::RunState::Enter(Monster* monster) {
	cout << "RunState Entered!" << endl;
    // 이동 시작
}

void MonsterState::RunState::Update(Monster* monster) {
	monster->_pos += monster->_velocity * TICK_INTERVAL; // 이동 처리
	if (monster->IsPlayerTooMuchClose()) {
        monster->SetState(S_MONSTER_STATE::SKILL);
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
	cout << "BasicAttackState Entered!" << endl;
}

void MonsterState::AttackState::Update(Monster* monster) {
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
    // hitTimer = 0.5f; // 피격 상태 애니메이션 시간 받아서 느야할듯?
    monster->SetVelocity(0, 0, 0); // 이동 멈춤

}

void MonsterState::HitState::Update(Monster* monster) {

}

void MonsterState::HitState::Exit(Monster* monster) {}



// MonsterState::DeathState 구현          =========================================================================



MonsterState::DeathState& MonsterState::DeathState::GetInstance() { static MonsterState::DeathState instance; return instance; }

void MonsterState::DeathState::Enter(Monster* monster) {
    monster->SetVelocity(0, 0, 0);
}

void MonsterState::DeathState::Update(Monster* monster) {
    // 사망 처리
}

void MonsterState::DeathState::Exit(Monster* monster) {}