#include "Monster.h"

void Monster::SetState(MonsterStateMachine* newState)
{
    //if (currentState) {
    //    if (newState != &MonsterState::HitState::GetInstance()) {
    //        previousState = currentState; // MonsterState::HitState로 갈 때는 현재 상태 저장
    //    }
    //    currentState->Exit(this);
    //}
    //currentState = newState;
    //if (currentState) currentState->Enter(this);
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
    Vec3 playerPos = GetPlayerPosition();
    float distance = (_pos - playerPos).Length();
    return distance < 2.0f; // 예: 2유닛 내
}

Vec3 Monster::GetPlayerPosition() const
{
    // 서버에서 플레이어 참조 (가정: 단일 플레이어)
    //extern PlayerCharacter* g_player; // 글로벌 플레이어 포인터 (가정)
    //if (g_player) return g_player->GetPosition();
    return Vec3::Zero;
}