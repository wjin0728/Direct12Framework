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

void Monster::SetState(S_MONSTER_STATE newState)
{
    switch (newState) {
    case S_MONSTER_STATE::IDLE:
        _state = S_MONSTER_STATE::IDLE;
        SetState(&MonsterState::IdleState::GetInstance());
        break;
    case S_MONSTER_STATE::RUN:
        _state = S_MONSTER_STATE::RUN;
        SetState(&MonsterState::RunState::GetInstance());
        break;
    case S_MONSTER_STATE::ATTACK:
        _state = S_MONSTER_STATE::ATTACK;
        SetState(&MonsterState::AttackState::GetInstance());
        break;
    case S_MONSTER_STATE::SKILL:
        _state = S_MONSTER_STATE::SKILL;
        SetState(&MonsterState::SkillState::GetInstance());
        break;
    case S_MONSTER_STATE::DEATH:
        _state = S_MONSTER_STATE::DEATH;
        SetState(&MonsterState::DeathState::GetInstance());
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

bool Monster::IsPlayerInRange(PlayerCharacter* target) const
{
	if (target == nullptr) return false; // Ÿ���� ������ false
	float distance = (_pos - target->_pos).LengthSquared();
    if (distance < pow(8.f, 2)) return true;
    return false;
}

bool Monster::IsPlayerTooMuchClose() const
{
    return (_target && (_pos - _target->_pos).LengthSquared() < pow(2.f, 2));
}

void Monster::SetTarget()
{
	float minDistance = 5000.f; // �� ū ��
    PlayerCharacter* close_player = nullptr;
	for (auto& player : _Player) {
		if (player == nullptr) continue; // �÷��̾ ������ �н�
        Vec3 playerPos = player->_pos;
		float distance = (_pos - playerPos).LengthSquared();
		// cout << distance << endl;
		if (distance < minDistance) {
			minDistance = distance;
            close_player = player;
		}
	}
	if (close_player && IsPlayerInRange(close_player)) {
		_target = close_player;
        Vec3 direction = _target->_pos - _pos;
        direction.y = 0.f;
        direction.Normalize();
        _look_dir = Vec3::Lerp(_look_dir, direction, 0.1f); // �ε巯�� ȸ��
        _velocity = direction * _speed; // Ÿ�� �������� �̵� �ӵ� ����
	}
	else {
		_target = nullptr; 
	}
}