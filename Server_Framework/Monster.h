#pragma once
#include "Object.h"
#include "MonsterState.h"
#include "PlayerCharacter.h"

struct EventKey
{
    float mTime = 0.0f; // 이벤스 발생 시점
    std::string mName; // 이벤트 이름
	bool mEnable = true; // 활성화 여부 (기본은 활성화. 현재 시간이 이벤트 발생 시점보다 뒤이면 이벤트 발생시키고 비활성화. 애니메이션 초기화시 true로 돌아감)
    // 이벤트 쓰려면 애니메이션 재생 시간 아는 변수 있어야됨.

    EventKey(float time, const std::string& name) : mTime(time), mName(name) {}
};

struct AnimationInfo {
    string mAnimationName; // 애니메이션 이름

	float mLength = 0.0f; // 애니메이션 길이 (초 단위)
	int mFrameLength = 0; // 애니메이션 프레임 길이

    ANIMATION_TYPE mType = ANIMATION_TYPE::LOOP; //Once, Loop, PingPong, End
	std::vector<std::shared_ptr<EventKey>> mEventKeys; // 이벤트 키 정보 (투사체 발사 등, 아직 안 넣음)
};

class Monster : public Object {
public:
    Vec3 _look_dir;
    Vec3 _speed{2, 0, 2};

    S_ENEMY_TYPE _class;
    S_MONSTER_STATE	_state;
	bool _remove = false;

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

	void ReadAnimationInfo(const std::string& fileName);

    void SetState(MonsterStateMachine* newState);
    void SetState(S_MONSTER_STATE newState);

    void Update();

    void TakeDamage(int damage);

    bool IsPlayerInRange(PlayerCharacter* target) const;
    bool IsPlayerTooMuchClose() const;

    void SetTarget();
};