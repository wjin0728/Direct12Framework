#include "Monster.h"
#include "PlayerCharacter.h"

void Monster::SetState(MonsterStateMachine* newState)
{
    if (currentState) {
        if (newState != &MonsterState::HitState::GetInstance()) {
            previousState = currentState; 
            // MonsterState::HitState로 갈 때는 현재 상태 저장
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
    LocalTransform(); // 바운딩 박스 업데이트 해주기
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
        // 사망
    }
}

bool Monster::IsPlayerInRange(PlayerCharacter* target) const
{
	if (target == nullptr) return false; // 타겟이 없으면 false
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
	float minDistance = 5000.f; // 걍 큰 수
    PlayerCharacter* close_player = nullptr;
	for (auto& player : _Player) {
		if (player == nullptr) continue; // 플레이어가 없으면 패스
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
        _look_dir = Vec3::Lerp(_look_dir, direction, 0.1f); // 부드러운 회전
        _velocity = direction * _speed; // 타겟 방향으로 이동 속도 설정
	}
	else {
		_target = nullptr; 
	}
}