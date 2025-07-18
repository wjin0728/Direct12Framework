#pragma once
#include "Object.h"
#include "MonsterState.h"
#include "PlayerCharacter.h"

class Monster : public Object {
public:
    Vec3 _look_dir;
    Vec3 _speed{2, 0, 2};

    S_ENEMY_TYPE _class;
    S_MONSTER_STATE	_state;
	bool _remove = false;
	bool _drop_item = false;

    int _hp;
    int _barrier;
    bool _on_FireEnchant;
    bool _on_GrassWeaken;

    MonsterStateMachine* currentState;
    S_MONSTER_STATE previousState;

    array<PlayerCharacter*, 3>_Player; // 플레이어 타겟
    PlayerCharacter* _target = nullptr;

    vector<std::shared_ptr<AnimationInfo>> _animations{}; // 애니메이션 정보들 (S_MONSTER_STATE 순서대로 들어감)

	Monster() :
        Object(S_OBJECT_TYPE::S_ENEMY),
        _class(S_ENEMY_TYPE::s_end),
        currentState(&MonsterState::IdleState::GetInstance()),
        _look_dir(Vec3(0, 0, 1)),
        _hp(100),
        _barrier(0),
        _on_FireEnchant(false),
        _on_GrassWeaken(false) {
    }
    Monster(S_ENEMY_TYPE monster_type) :
        Object(S_OBJECT_TYPE::S_ENEMY),
        _class(monster_type),
        currentState(&MonsterState::IdleState::GetInstance()),
        _look_dir(Vec3(0, 0, 1)),
        _hp(100),
        _barrier(0),
        _on_FireEnchant(false),
        _on_GrassWeaken(false) {
        switch (monster_type) {
        case S_ENEMY_TYPE::FIRE_SMALL: {
			ReadAnimationInfo("Animations/FireSmall.bin");
            break;
        }
        case S_ENEMY_TYPE::FIRE_BIG: {
            ReadAnimationInfo("Animations/FireBig.bin");
            break;
        }
        case S_ENEMY_TYPE::WATER_SMALL: {
            ReadAnimationInfo("Animations/WaterSmall.bin");
            break;
        }
        case S_ENEMY_TYPE::WATER_BIG: {
            ReadAnimationInfo("Animations/WaterBig.bin");
            break;
        }
        case S_ENEMY_TYPE::GRASS_SMALL: {
            ReadAnimationInfo("Animations/GrassSmall.bin");
            _boundingbox.Center = XMFLOAT3(0, 0.83, 0);
            _boundingbox.Extents = Vec3(1, 1, 1) / 2.f;
            break;
        }
        case S_ENEMY_TYPE::GRASS_BIG: {
            ReadAnimationInfo("Animations/GrassBig.bin");
            _boundingbox.Center = XMFLOAT3(0, 1.69, 0);
            _boundingbox.Extents = Vec3(1.79, 2.95, 1.42) / 2.f;
            break;
        }
        }
        _active = false;
    }

    void AvoidCollision(const unordered_map<int, Monster>& monsters);

    void ReadAnimationInfo(const std::string& fileName);

    void SetState(MonsterStateMachine* newState);
    void SetState(S_MONSTER_STATE newState);

    void Update();

    void TakeDamage(int damage);

    bool IsPlayerInRange(PlayerCharacter* target) const;
    bool IsPlayerTooMuchClose() const;

    void SetTarget();
};