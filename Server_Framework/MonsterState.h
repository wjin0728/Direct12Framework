#ifndef MONSTERSTATEMACHINE_H
#define MONSTERSTATEMACHINE_H

#include "stdafx.h"

// 전방 선언
class Monster;

// 상태 머신의 추상 기본 클래스
class MonsterStateMachine {
public:
    virtual ~MonsterStateMachine() {}
    virtual void Enter(Monster* monster) = 0;
    virtual void Update(Monster* monster) = 0;
    virtual void Exit(Monster* monster) = 0;
};

namespace MonsterState
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
    };

    // Run 상태
    class RunState : public MonsterStateMachine {
    public:
        static RunState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        RunState() {}
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
        float hitTimer;
    };
}

#endif