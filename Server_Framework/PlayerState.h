
#ifndef PLAYERCHARACTER_H
#define PLAYERCHARACTER_H

#include "stdafx.h"

// ���� ����
class PlayerCharacter;

// ���� �ӽ��� �߻� �⺻ Ŭ����
class PlayerStateMachine {
public:
    virtual ~PlayerStateMachine() {}
    virtual void Enter(PlayerCharacter* player) = 0;
    virtual void Update(PlayerCharacter* player) = 0;
    virtual void Exit(PlayerCharacter* player) = 0;
};

namespace PlayerState
{
    // Idle ����
    class IdleState : public PlayerStateMachine {
    public:
        static IdleState& GetInstance();
        void Enter(PlayerCharacter* player) override;
        void Update(PlayerCharacter* player) override;
        void Exit(PlayerCharacter* player) override;
    private:
        IdleState() {}
    };

    // Run ����
    class RunState : public PlayerStateMachine {
    public:
        static RunState& GetInstance();
        void Enter(PlayerCharacter* player) override;
        void Update(PlayerCharacter* player) override;
        void Exit(PlayerCharacter* player) override;
    private:
        RunState() {}
    };

    // BasicAttack ����
    class BasicAttackState : public PlayerStateMachine {
    public:
        static BasicAttackState& GetInstance();
        void Enter(PlayerCharacter* player) override;
        void Update(PlayerCharacter* player) override;
        void Exit(PlayerCharacter* player) override;
    private:
        BasicAttackState() {}
        float attackTimer;
    };

    // RunAttack ����
    class RunAttackState : public PlayerStateMachine {
    public:
        static RunAttackState& GetInstance();
        void Enter(PlayerCharacter* player) override;
        void Update(PlayerCharacter* player) override;
        void Exit(PlayerCharacter* player) override;
    private:
        RunAttackState() {}
        float attackTimer;
    };

    // Jump ����
    class JumpState : public PlayerStateMachine {
    public:
        static JumpState& GetInstance();
        void Enter(PlayerCharacter* player) override;
        void Update(PlayerCharacter* player) override;
        void Exit(PlayerCharacter* player) override;
    private:
        JumpState() {}
        float attackTimer;
    };

    // Skill ����
    class SkillState : public PlayerStateMachine {
    public:
        static SkillState& GetInstance();
        void Enter(PlayerCharacter* player) override;
        void Update(PlayerCharacter* player) override;
        void Exit(PlayerCharacter* player) override;
    private:
        SkillState() {}
        float skillTimer;
    };

    // Hit ����
    class HitState : public PlayerStateMachine {
    public:
        static HitState& GetInstance();
        void Enter(PlayerCharacter* player) override;
        void Update(PlayerCharacter* player) override;
        void Exit(PlayerCharacter* player) override;
    private:
        HitState() {}
        float hitTimer;
    };
}

#endif