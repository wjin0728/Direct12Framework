#ifndef BossMonsterStateMachine_H
#define BossMonsterStateMachine_H

#include "stdafx.h"
#include "Monster.h"

// 전방 선언
class Monster;

namespace BossMonsterState
{
    // Idle 상태
    class IdleState : public MonsterStateMachine {
    public:
        static IdleState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        IdleState() {}
        float idleTimer = 0.0f;
    };

    // Targeting 상태
    class TargetingState : public MonsterStateMachine {
    public:
        static TargetingState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        TargetingState() {}
        float targetingTimer = 0.0f; // 타겟팅 타이머
        float targetingDelay = 0.0f; // 타겟팅 딜레이 시간
    };

    // BasicAttack 상태
    class AttackState : public MonsterStateMachine {
    public:
        static AttackState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        AttackState() {}
        float attackTimer;
    };

    // CastingState 상태
    class CastingState : public MonsterStateMachine {
    public:
        static CastingState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        CastingState() {}
        float skillTimer;
    };
    

    // SkillAttack 상태
    class SkillState : public MonsterStateMachine {
    public:
        static SkillState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        SkillState() {}
        float skillTimer;
    };

    // Hit 상태
    class HitState : public MonsterStateMachine {
    public:
        static HitState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        HitState() {}
        float hitTimer;
    };

    // Death 상태
    class DeathState : public MonsterStateMachine {
    public:
        static DeathState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        DeathState() {}
        float deathTimer;
    };

    // underground 상태
    class UndergroundState : public MonsterStateMachine {
    public:
        static UndergroundState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        UndergroundState() {}
        float UndergroundTimer = 0.f;
    };

    // Spawn 상태
    class SpawnState : public MonsterStateMachine {
    public:
        static SpawnState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        SpawnState() {}
        float SpawnTimer;
    };
}

#endif