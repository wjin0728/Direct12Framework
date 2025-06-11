#include "MonsterState.h"
#include "Monster.h"

// MonsterState::IdleState ����         =========================================================================



MonsterState::IdleState& MonsterState::IdleState::GetInstance() { static MonsterState::IdleState instance; return instance; }

void MonsterState::IdleState::Enter(Monster* monster) {
	cout << "IdleState Entered!" << endl;
    monster->SetVelocity(0, 0, 0); // �ӵ� 0
}

void MonsterState::IdleState::Update(Monster* monster) {
    if (monster->_target) {
        monster->SetState(S_MONSTER_STATE::RUN);
    }
}

void MonsterState::IdleState::Exit(Monster* monster) {}



// MonsterState::RunState ����          =========================================================================



MonsterState::RunState& MonsterState::RunState::GetInstance() { static MonsterState::RunState instance; return instance; }

void MonsterState::RunState::Enter(Monster* monster) {
	cout << "RunState Entered!" << endl;
    // �̵� ����
}

void MonsterState::RunState::Update(Monster* monster) {
	monster->_pos += monster->_velocity * TICK_INTERVAL; // �̵� ó��
	if (monster->IsPlayerTooMuchClose()) {
		monster->SetState(S_MONSTER_STATE::ATTACK);
		monster->_target = nullptr; // Ÿ�� �ʱ�ȭ �̰Ŵ� �ӽ÷� �صа���
	}
}

void MonsterState::RunState::Exit(Monster* monster) {}



// MonsterState::AttackState ����  =========================================================================



MonsterState::AttackState& MonsterState::AttackState::GetInstance() { static MonsterState::AttackState instance; return instance; }

void MonsterState::AttackState::Enter(Monster* monster) {
	cout << "BasicAttackState Entered!" << endl;
}

void MonsterState::AttackState::Update(Monster* monster) {
}

void MonsterState::AttackState::Exit(Monster* monster) {}



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