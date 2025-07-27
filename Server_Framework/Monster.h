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
    bool _on_CantMove;
	float cant_move_time = 0.f;

    MonsterStateMachine* currentState;
    S_MONSTER_STATE previousState;

    array<PlayerCharacter*, 3>_Player; // 플레이어 타겟
    PlayerCharacter* _target = nullptr;
	Vec3 _attack_pos = Vec3(0, 0, 0); // 보스 기본공격 위치

    vector<AnimationInfo> _animations{}; // 애니메이션 정보들 (S_MONSTER_STATE 순서대로 들어감)
    vector<CAnimationEventHandler> mEventHandler;
    float _animation_time = 0.f;

	Monster() :
        Object(S_OBJECT_TYPE::S_ENEMY),
        _class(S_ENEMY_TYPE::s_end),
        currentState(&MonsterState::UndergroundState::GetInstance()),
		_state(S_MONSTER_STATE::UNDERGROUND),
        _look_dir(Vec3(0, 0, 1)),
        _hp(100),
        _barrier(0),
        _on_FireEnchant(false),
        _on_CantMove(false) {
    }
    Monster(S_ENEMY_TYPE monster_type) :
        Object(S_OBJECT_TYPE::S_ENEMY),
        _class(monster_type),
        currentState(&MonsterState::UndergroundState::GetInstance()),
        _state(S_MONSTER_STATE::UNDERGROUND),
        _look_dir(Vec3(0, 0, 1)),
        _barrier(0),
        _on_FireEnchant(false),
        _on_CantMove(false)
    {
        switch (monster_type) {
        case S_ENEMY_TYPE::FIRE_SMALL: {
			ReadAnimationInfo("Animations/FireSmall.bin");
            _hp = 50.f;
            break;
        }
        case S_ENEMY_TYPE::FIRE_BIG: {
            ReadAnimationInfo("Animations/FireBig.bin");
            _hp = 100.f;
            break;
        }
        case S_ENEMY_TYPE::WATER_SMALL: {
            ReadAnimationInfo("Animations/WaterSmall.bin");
            _hp = 50.f;
            break;
        }
        case S_ENEMY_TYPE::WATER_BIG: {
            ReadAnimationInfo("Animations/WaterBig.bin");
            _hp = 100.f;
            break;
        }
        case S_ENEMY_TYPE::GRASS_SMALL: {
            ReadAnimationInfo("Animations/GrassSmall.bin");
            _orignalboundingbox.Center = XMFLOAT3(0, 0.83, 0);
            _orignalboundingbox.Extents = Vec3(0.73, 1.27, 0.76) / 2.f;
            _hp = 50.f;
            break;
        }
        case S_ENEMY_TYPE::GRASS_BIG: {
            ReadAnimationInfo("Animations/GrassBig.bin");
            _orignalboundingbox.Center = XMFLOAT3(0, 1.72, 0);
            _orignalboundingbox.Extents = Vec3(1.61, 2.71, 1.21) / 2.f;
            _hp = 100.f;
            break;
        }
        case S_ENEMY_TYPE::BOSS: {
            ReadAnimationInfo("Animations/Boss.bin");
            _orignalboundingbox.Center = XMFLOAT3(0, 1.91, 0);
            _orignalboundingbox.Extents = Vec3(1.87, 3.84, 1.51) / 2.f;
            _hp = 300.f;
            break;
        }
        }
        //_active = false;
    }

    void AvoidCollision(const unordered_map<int, Monster>& monsters);

    void ReadAnimationInfo(const std::string& fileName);
    void HandleCallback(CAnimationEventHandler& registry);
    void AddAnimationEvent(S_MONSTER_STATE state, const std::string& name, CAnimationEventHandler::Event event) { mEventHandler[(int)state].Register(name, event); }

    void SetState(MonsterStateMachine* newState);
    void SetState(S_MONSTER_STATE newState);
    void SetMonsterState(S_MONSTER_STATE newState);
    void SetBossState(S_MONSTER_STATE newState);

    void Update();

    void TakeDamage(int damage, bool do_hit_raction);

    bool IsPlayerInRange(PlayerCharacter* target) const;
    bool IsPlayerTooMuchClose() const;

    void UpdateTarget();
	void SetRandomTarget();
	void ResetTarget() { _target = nullptr; }

    Vec2 GetWorldOffsetPosition(float local_x, float local_z) {
        Vec3 forward = _look_dir;
        forward.y = 0.0f;
        forward.Normalize();

        Vec3 right = Vec3::Up.Cross(forward);
        right.Normalize();

        Vec3 result = right * local_x + forward * local_z + _pos;

        return {result.x, result.z};
    }

    bool IsUnavailable() {
        return (_remove || _state == S_MONSTER_STATE::UNDERGROUND || _state == S_MONSTER_STATE::DEATH || _state == S_MONSTER_STATE::SPAWN);
    }
};