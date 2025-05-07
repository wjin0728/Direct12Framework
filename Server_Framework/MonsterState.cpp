#include "MonsterState.h"
#include "Monster.h"

// IdleState 구현         =========================================================================



IdleState& IdleState::GetInstance() { static IdleState instance; return instance; }

void IdleState::Enter(Monster* monster) {
    monster->SetVelocity(0, 0, 0); // 속도 0
}

void IdleState::Update(Monster* monster) {
    if (monster->IsPlayerInRange()) {
        monster->SetState(&RunState::GetInstance());
    }
}

void IdleState::Exit(Monster* monster) {}



// RunState 구현          =========================================================================



RunState& RunState::GetInstance() { static RunState instance; return instance; }

void RunState::Enter(Monster* monster) {
    // 이동 시작
}

void RunState::Update(Monster* monster) {
    Vec3 playerPos = monster->GetPlayerPosition();
    Vec3 direction = playerPos - monster->_pos;
    direction.Normalize();
    float speed = 5.0f;
    monster->SetVelocity(direction.x * speed, direction.y * speed, direction.z * speed);
    monster->SetPosition(
        monster->_pos.x + monster->_velocity.x * TICK_INTERVAL,
        monster->_pos.y + monster->_velocity.y * TICK_INTERVAL,
        monster->_pos.z + monster->_velocity.z * TICK_INTERVAL
    );
    if (monster->IsCloseToPlayer()) {
        monster->SetState(&BasicAttackState::GetInstance());
    }
}

void RunState::Exit(Monster* monster) {}



// BasicAttackState 구현  =========================================================================



BasicAttackState& BasicAttackState::GetInstance() { static BasicAttackState instance; return instance; }

void BasicAttackState::Enter(Monster* monster) {
    attackTimer = 1.0f; // 공격 지속 시간
}

void BasicAttackState::Update(Monster* monster) {
    attackTimer -= TICK_INTERVAL;
    if (attackTimer <= 0) {
        monster->SetState(&IdleState::GetInstance());
    }
}

void BasicAttackState::Exit(Monster* monster) {}



// SkillAttackState 구현  =========================================================================



SkillAttackState& SkillAttackState::GetInstance() { static SkillAttackState instance; return instance; }

void SkillAttackState::Enter(Monster* monster) {
    skillTimer = 2.0f; // 스킬 지속 시간
}

void SkillAttackState::Update(Monster* monster) {
    skillTimer -= TICK_INTERVAL;
    if (skillTimer <= 0) {
        monster->SetState(&IdleState::GetInstance());
    }
}

void SkillAttackState::Exit(Monster* monster) {}



// HitState 구현          =========================================================================



HitState& HitState::GetInstance() { static HitState instance; return instance; }

void HitState::Enter(Monster* monster) {
    // hitTimer = 0.5f; // 피격 상태 애니메이션 시간 받아서 느야할듯?
    monster->SetVelocity(0, 0, 0); // 이동 멈춤

}

void HitState::Update(Monster* monster) {
    hitTimer -= TICK_INTERVAL;
    if (hitTimer <= 0) {
        // 이전 상태 또는 상황에 따라 복귀
        if (monster->IsPlayerInRange() && !monster->IsCloseToPlayer()) {
            monster->SetState(&RunState::GetInstance());
        }
        else if (monster->IsCloseToPlayer()) {
            monster->SetState(&BasicAttackState::GetInstance());
        }
        else {
            monster->SetState(&IdleState::GetInstance());
        }
    }
}

void HitState::Exit(Monster* monster) {}