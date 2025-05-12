
#ifndef PLAYERCHARACTER_H
#define PLAYERCHARACTER_H

#include "stdafx.h"

// 전방 선언
class PlayerCharacter;

// 상태 머신의 추상 기본 클래스
class PlayerStateMachine {
public:
    virtual ~PlayerStateMachine() {}
    virtual void Enter(PlayerCharacter* player) = 0;
    virtual void Update(PlayerCharacter* player) = 0;
    virtual void Exit(PlayerCharacter* player) = 0;
};

namespace PlayerState
{
    // Idle 상태
    class IdleState : public PlayerStateMachine {
    public:
        static IdleState& GetInstance();
        void Enter(PlayerCharacter* player) override;
        void Update(PlayerCharacter* player) override;
        void Exit(PlayerCharacter* player) override;
    private:
        IdleState() {}
    };

    // Run 상태
    class RunState : public PlayerStateMachine {
    public:
        static RunState& GetInstance();
        void Enter(PlayerCharacter* player) override;
        void Update(PlayerCharacter* player) override;
        void Exit(PlayerCharacter* player) override;
    private:
        RunState() {}
    };

    // BasicAttack 상태
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

    // RunAttack 상태
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

    // Jump 상태
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

    // Skill 상태
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

    // Hit 상태
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