#include "MonsterState.h"
#include "Monster.h"

// MonsterState::IdleState ����         =========================================================================



MonsterState::IdleState& MonsterState::IdleState::GetInstance() { static MonsterState::IdleState instance; return instance; }

void MonsterState::IdleState::Enter(Monster* monster) {
    monster->SetVelocity(0, 0, 0); // �ӵ� 0
}

void MonsterState::IdleState::Update(Monster* monster) {
    if (monster->IsPlayerInRange()) {
        monster->SetState(&MonsterState::RunState::GetInstance());
    }
}

void MonsterState::IdleState::Exit(Monster* monster) {}



// MonsterState::RunState ����          =========================================================================



MonsterState::RunState& MonsterState::RunState::GetInstance() { static MonsterState::RunState instance; return instance; }

void MonsterState::RunState::Enter(Monster* monster) {
    // �̵� ����
}

void MonsterState::RunState::Update(Monster* monster) {
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
        monster->SetState(&MonsterState::BasicAttackState::GetInstance());
    }
}

void MonsterState::RunState::Exit(Monster* monster) {}



// MonsterState::BasicAttackState ����  =========================================================================



MonsterState::BasicAttackState& MonsterState::BasicAttackState::GetInstance() { static MonsterState::BasicAttackState instance; return instance; }

void MonsterState::BasicAttackState::Enter(Monster* monster) {
    attackTimer = 1.0f; // ���� ���� �ð�
}

void MonsterState::BasicAttackState::Update(Monster* monster) {
    attackTimer -= TICK_INTERVAL;
    if (attackTimer <= 0) {
        monster->SetState(&MonsterState::IdleState::GetInstance());
    }
}

void MonsterState::BasicAttackState::Exit(Monster* monster) {}



// MonsterState::SkillState ����  =========================================================================



MonsterState::SkillState& MonsterState::SkillState::GetInstance() { static MonsterState::SkillState instance; return instance; }

void MonsterState::SkillState::Enter(Monster* monster) {
    skillTimer = 2.0f; // ��ų ���� �ð� �������ֱ�
}

void MonsterState::SkillState::Update(Monster* monster) {
    skillTimer -= TICK_INTERVAL;
    if (skillTimer <= 0) {
        monster->SetState(&MonsterState::IdleState::GetInstance());
    }
}

void MonsterState::SkillState::Exit(Monster* monster) {}



// MonsterState::HitState ����          =========================================================================



MonsterState::HitState& MonsterState::HitState::GetInstance() { static MonsterState::HitState instance; return instance; }

void MonsterState::HitState::Enter(Monster* monster) {
    // hitTimer = 0.5f; // �ǰ� ���� �ִϸ��̼� �ð� �޾Ƽ� �����ҵ�?
    monster->SetVelocity(0, 0, 0); // �̵� ����

}

void MonsterState::HitState::Update(Monster* monster) {
    hitTimer -= TICK_INTERVAL;
    if (hitTimer <= 0) {
        // ���� ���� �Ǵ� ��Ȳ�� ���� ����
        if (monster->IsPlayerInRange() && !monster->IsCloseToPlayer()) {
            monster->SetState(&MonsterState::RunState::GetInstance());
        }
        else if (monster->IsCloseToPlayer()) {
            monster->SetState(&MonsterState::BasicAttackState::GetInstance());
        }
        else {
            monster->SetState(&MonsterState::IdleState::GetInstance());
        }
    }
}

void MonsterState::HitState::Exit(Monster* monster) {}



// MonsterState::DeathState ����          =========================================================================



MonsterState::DeathState& MonsterState::DeathState::GetInstance() { static MonsterState::DeathState instance; return instance; }

void MonsterState::DeathState::Enter(Monster* monster) {
    monster->SetVelocity(0, 0, 0);
}

void MonsterState::DeathState::Update(Monster* monster) {
    // ��� ó��
}

void MonsterState::DeathState::Exit(Monster* monster) {}