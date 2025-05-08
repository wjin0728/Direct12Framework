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
    class BasicAttackState : public MonsterStateMachine {
    public:
        static BasicAttackState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        BasicAttackState() {}
        float attackTimer;
    };

    // SkillAttack ����
    class SkillAttackState : public MonsterStateMachine {
    public:
        static SkillAttackState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        SkillAttackState() {}
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
}

#endif