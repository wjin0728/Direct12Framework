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
    LocalTransform(); // 바운딩 박스 업데이트 해주기
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

bool Monster::IsPlayerInRange() const
{
    Vec3 playerPos = GetPlayerPosition();
    float distance = (_pos - playerPos).Length();
    return distance < 10.0f; // 예: 10유닛 내
}

bool Monster::IsCloseToPlayer() const
{
    //Vec3 playerPos = GetPlayerPosition();
    //float distance = (_pos - playerPos).Length();
    return 0; // distance < 2.0f; // 예: 2유닛 내
}

Vec3 Monster::GetPlayerPosition() const
{
    return Vec3();
}
