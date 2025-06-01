#pragma once
#include "Object.h"
#include "MonsterState.h"

class PlayerCharater;

class Monster : public Object {
public:
    Vec3 _look_dir;
    Vec3 _acceleration;

    S_ENEMY_TYPE _class;
    S_MONSTER_STATE	_state;

    int _hp;
    int _barrier;
    bool _on_FireEnchant;
    bool _on_GrassWeaken;

	int _targetId;

    MonsterStateMachine* currentState;
    MonsterStateMachine* previousState;

	Monster() :
        Object(S_OBJECT_TYPE::S_ENEMY),
        _class(S_ENEMY_TYPE::s_end),
        currentState(&MonsterState::IdleState::GetInstance()),
        previousState(nullptr),
        _look_dir(Vec3(0, 0, 1)),
        _acceleration(Vec3::Zero),
        _hp(100),
        _barrier(0),
        _on_FireEnchant(false),
        _on_GrassWeaken(false) {
    }

    Monster(S_ENEMY_TYPE monster_type) :
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
    void SetState(uint8_t newState);

    void Update();

    void TakeDamage(int damage);

    bool IsPlayerInRange() const;

    bool IsCloseToPlayer() const;

    Vec3 GetPlayerPosition() const;
};