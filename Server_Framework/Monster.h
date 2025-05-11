#pragma once
#include "Object.h"
#include "MonsterState.h"

class Monster : public Object {
public:
    Vec3 _look_dir;
    Vec3 _acceleration;
    S_MONSTER_TYPE _class;
    int _hp;
    int _barrier;
    bool _on_FireEnchant;
    bool _on_GrassWeaken;
    MonsterStateMachine* currentState;
    MonsterStateMachine* previousState;

    Monster(S_MONSTER_TYPE monster_type) :
        Object(S_OBJECT_TYPE::S_ENEMY),
        _class(monster_type),
        currentState(&MonsterState::IdleState::GetInstance()),
        previousState(nullptr),
        _look_dir(Vec3(0, 0, 1)),
        _acceleration(Vec3::Zero),
        _hp(100),
        _barrier(0),
        _on_FireEnchant(false),
        _on_GrassWeaken(false) {
    }

    void SetState(MonsterStateMachine* newState);

    void Update();

    void TakeDamage(int damage);

    bool IsPlayerInRange() const;

    bool IsCloseToPlayer() const;

    Vec3 GetPlayerPosition() const;
};