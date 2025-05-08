#include "Monster.h"

void Monster::SetState(MonsterStateMachine* newState)
{
    //if (currentState) {
    //    if (newState != &MonsterState::HitState::GetInstance()) {
    //        previousState = currentState; // MonsterState::HitState�� �� ���� ���� ���� ����
    //    }
    //    currentState->Exit(this);
    //}
    //currentState = newState;
    //if (currentState) currentState->Enter(this);
}

void Monster::Update()
{
    if (currentState) currentState->Update(this);
    LocalTransform(); // �ٿ�� �ڽ� ������Ʈ ���ֱ�
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
    Vec3 playerPos = GetPlayerPosition();
    float distance = (_pos - playerPos).Length();
    return distance < 10.0f; // ��: 10���� ��
}

bool Monster::IsCloseToPlayer() const
{
    Vec3 playerPos = GetPlayerPosition();
    float distance = (_pos - playerPos).Length();
    return distance < 2.0f; // ��: 2���� ��
}

Vec3 Monster::GetPlayerPosition() const
{
    // �������� �÷��̾� ���� (����: ���� �÷��̾�)
    //extern PlayerCharacter* g_player; // �۷ι� �÷��̾� ������ (����)
    //if (g_player) return g_player->GetPosition();
    return Vec3::Zero;
}