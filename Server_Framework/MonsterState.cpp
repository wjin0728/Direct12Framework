#include "MonsterState.h"
#include "Monster.h"

// IdleState ����         =========================================================================



IdleState& IdleState::GetInstance() { static IdleState instance; return instance; }

void IdleState::Enter(Monster* monster) {
    monster->SetVelocity(0, 0, 0); // �ӵ� 0
}

void IdleState::Update(Monster* monster) {
    if (monster->IsPlayerInRange()) {
        monster->SetState(&RunState::GetInstance());
    }
}

void IdleState::Exit(Monster* monster) {}



// RunState ����          =========================================================================



RunState& RunState::GetInstance() { static RunState instance; return instance; }

void RunState::Enter(Monster* monster) {
    // �̵� ����
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



// BasicAttackState ����  =========================================================================



BasicAttackState& BasicAttackState::GetInstance() { static BasicAttackState instance; return instance; }

void BasicAttackState::Enter(Monster* monster) {
    attackTimer = 1.0f; // ���� ���� �ð�
}

void BasicAttackState::Update(Monster* monster) {
    attackTimer -= TICK_INTERVAL;
    if (attackTimer <= 0) {
        monster->SetState(&IdleState::GetInstance());
    }
}

void BasicAttackState::Exit(Monster* monster) {}



// SkillAttackState ����  =========================================================================



SkillAttackState& SkillAttackState::GetInstance() { static SkillAttackState instance; return instance; }

void SkillAttackState::Enter(Monster* monster) {
    skillTimer = 2.0f; // ��ų ���� �ð�
}

void SkillAttackState::Update(Monster* monster) {
    skillTimer -= TICK_INTERVAL;
    if (skillTimer <= 0) {
        monster->SetState(&IdleState::GetInstance());
    }
}

void SkillAttackState::Exit(Monster* monster) {}



// HitState ����          =========================================================================



HitState& HitState::GetInstance() { static HitState instance; return instance; }

void HitState::Enter(Monster* monster) {
    // hitTimer = 0.5f; // �ǰ� ���� �ִϸ��̼� �ð� �޾Ƽ� �����ҵ�?
    monster->SetVelocity(0, 0, 0); // �̵� ����

}

void HitState::Update(Monster* monster) {
    hitTimer -= TICK_INTERVAL;
    if (hitTimer <= 0) {
        // ���� ���� �Ǵ� ��Ȳ�� ���� ����
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