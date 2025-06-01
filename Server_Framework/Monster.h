#pragma once
#include "Object.h"
#include "MonsterState.h"
#include "PlayerCharacter.h"


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

    MonsterStateMachine* currentState;
    MonsterStateMachine* previousState;

    array<PlayerCharacter*, 3>_Player; // 플레이어 타겟
    PlayerCharacter* _target = nullptr;

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
        switch (monster_type) {
        case S_ENEMY_TYPE::GRASS_SMALL: {
            _boundingbox.Center = XMFLOAT3(0, 0.83, 0);
            _boundingbox.Extents = Vec3(1, 1, 1) / 2.f;
            break;
        }
        case S_ENEMY_TYPE::GRASS_BIG: {
            _boundingbox.Center = XMFLOAT3(0, 1.69, 0);
            _boundingbox.Extents = Vec3(1.79, 2.95, 1.42) / 2.f;
            break;
        }
        }
        _active = false;
    }

    void SetState(MonsterStateMachine* newState);
    void SetState(uint8_t newState);

    void Update();

    void TakeDamage(int damage);

    bool IsPlayerInRange() const;

    void SetTarget();
};