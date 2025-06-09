#ifndef MONSTERSTATEMACHINE_H
#define MONSTERSTATEMACHINE_H

#include "stdafx.h"

// ���� ����
class Monster;

// ���� �ӽ��� �߻� �⺻ Ŭ����
class MonsterStateMachine {
public:
    virtual ~MonsterStateMachine() {}
    virtual void Enter(Monster* monster) = 0;
    virtual void Update(Monster* monster) = 0;
    virtual void Exit(Monster* monster) = 0;
};

namespace MonsterState
{
    // Idle ����
    class IdleState : public MonsterStateMachine {
    public:
        static IdleState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        IdleState() {}
    };

    // Run ����
    class RunState : public MonsterStateMachine {
    public:
        static RunState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        RunState() {}
    };

    // BasicAttack ����
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

    // SkillAttack ����
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

    // Hit ����
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

    // Death ����
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