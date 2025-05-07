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
    MonsterState* currentState;
    MonsterState* previousState;

    Monster(S_MONSTER_TYPE monster_type) :
        Object(S_OBJECT_TYPE::S_ENEMY),
        _class(monster_type),
        currentState(&IdleState::GetInstance()),
        previousState(nullptr),
        _look_dir(Vec3(0, 0, 1)),
        _acceleration(Vec3::Zero),
        _hp(100),
        _barrier(0),
        _on_FireEnchant(false),
        _on_GrassWeaken(false) {
    }

    void SetState(MonsterState* newState) {
        if (currentState) {
            if (newState != &HitState::GetInstance()) {
                previousState = currentState; // HitState로 갈 때는 현재 상태 저장
            }
            currentState->Exit(this);
        }
        currentState = newState;
        if (currentState) currentState->Enter(this);
    }

    void Update() {
        if (currentState) currentState->Update(this);
        LocalTransform(); // 바운딩 박스 업데이트 해주기
    }

    void TakeDamage(int damage) {
        if (_barrier > 0) {
            _barrier -= damage;
            if (_barrier < 0) _barrier = 0;
        }
        else {
            _hp -= damage;
            if (_hp < 0) _hp = 0;
        }
        if (_hp > 0) {
            SetState(&HitState::GetInstance());
        }
        else {
            // 사망
        }
    }

    bool IsPlayerInRange() const {

        return false;
    }

    bool IsCloseToPlayer() const {

        return false;
    }

    Vec3 GetPlayerPosition() const {

        return Vec3::Zero;
    }
};