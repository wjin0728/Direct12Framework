#include "Monster.h"
#include "PlayerCharacter.h"

void Monster::SetState(MonsterStateMachine* newState)
{
    if (currentState) {
        if (newState != &MonsterState::HitState::GetInstance()) {
            previousState = currentState; 
            // MonsterState::HitState�� �� ���� ���� ���� ����
        }
        currentState->Exit(this);
    }
    currentState = newState;
    if (currentState) currentState->Enter(this);
}

void Monster::SetState(uint8_t newState)
{
    switch (newState) {
    case (uint8_t)S_MONSTER_STATE::IDLE:
        _state = S_MONSTER_STATE::IDLE;
        SetState(&MonsterState::IdleState::GetInstance());
        break;
    case (uint8_t)S_MONSTER_STATE::RUN:
        _state = S_MONSTER_STATE::RUN;
        SetState(&MonsterState::RunState::GetInstance());
        break;
    case (uint8_t)S_MONSTER_STATE::ATTACK:
        _state = S_MONSTER_STATE::ATTACK;
        SetState(&MonsterState::BasicAttackState::GetInstance());
        break;
    case (uint8_t)S_MONSTER_STATE::SKILL:
        _state = S_MONSTER_STATE::SKILL;
        SetState(&MonsterState::SkillState::GetInstance());
        break;
    default:
        break;
    }
}

void Monster::Update()
{
    if (currentState) currentState->Update(this);
    LocalTransform(); // �ٿ�� �ڽ� ������Ʈ ���ֱ�
    SetTarget();
}

void Monster::TakeDamage(int damage)
{
    if (_barrier > 0) {
        _barrier -= damage;
        if (_barrier < 0) _barrier = 0;
    }
    else {
        _hp -= damage;
        if (_hp < 0) _hp = 0;
    }
    if (_hp > 0) {
        SetState(&MonsterState::HitState::GetInstance());
    }
    else {
        // ���
    }
}

bool Monster::IsPlayerInRange() const
{
    //Vec3 playerPos = GetPlayerPosition();
    //float distance = (_pos - playerPos).Length();
    //return distance < 10.0f; // ��: 10���� ��
    return 0;
}

void Monster::SetTarget()
{
	float minDistance = 5000.f; // �� ū ��
	for (auto& player : _Player) {
		if (player == nullptr) continue; // �÷��̾ ������ �н�
        Vec3 playerPos = player->_pos;
		float distance = (_pos - playerPos).LengthSquared();
		cout << distance << endl;
		if (distance < minDistance) {
			minDistance = distance;
            _target = player;
			cout << "���� " << (int)_class << "�� �÷��̾� " << (int)player->_class << "�� Ÿ�����߽��ϴ�." << endl;
		}
	}

	if (_target) {
		Vec3 direction = _target->_pos - _pos;
		direction.y = 0.f;
		direction.Normalize();
		_look_dir = Vec3::Lerp(_look_dir, direction, 0.1f); // �ε巯�� ȸ��
	}
}