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

    int pattern_cnt = 3;
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
		float idleTimer = 0.0f; // 기본 대기 시간
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

	/////////////////보스 상태 머신/////////////////

    // Idle 상태
    class BossIdleState : public MonsterStateMachine {
    public:
        static BossIdleState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        BossIdleState() {}
        float idleTimer = 0.0f;
    };

    // Targeting 상태
    class BossTargetingState : public MonsterStateMachine {
    public:
        static BossTargetingState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;

        bool GetSendTarget() const { return sendTarget; }
        void SetSendTarget(bool value) { sendTarget = value; }
        bool GetSendTargetLock() const { return sendTargetLock; }
        void SetSendTargetLock(bool value) { sendTargetLock = value; }
    private:
        BossTargetingState() {}
        float targetingTimer = 0.0f; // 타겟팅 타이머
        float targetingDelay = 0.0f; // 타겟팅 딜레이 시간
        bool sendTarget = false;
        bool sendTargetLock = false;
    };

    // BasicAttack 상태
    class BossAttackState : public MonsterStateMachine {
    public:
        static BossAttackState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        BossAttackState() {}
        float attackTimer;
    };

    // SkillAttack 상태
    class BossSkillState : public MonsterStateMachine {
    public:
        static BossSkillState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;

		S_ITEM_TYPE GetSkillType() const { return skillType; }
		void SetSkillType(S_ITEM_TYPE type) { skillType = type; }
		bool GetSendSkill() const { return sendSkill; }
		void SetSendSkill(bool value) { sendSkill = value; }

		std::vector<int> hit_client_id; // 스킬 맞은 플레이어 ID들

    private:
        BossSkillState() {}
        float skillTimer;
		S_ITEM_TYPE skillType = S_ITEM_TYPE::S_item_end; // 스킬 타입
        bool sendSkill = false;
    };

    // Hit 상태
    class BossHitState : public MonsterStateMachine {
    public:
        static BossHitState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        BossHitState() {}
        float hitTimer;
    };

    // Death 상태
    class BossDeathState : public MonsterStateMachine {
    public:
        static BossDeathState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        BossDeathState() {}
        float deathTimer;
    };

    // underground 상태
    class BossUndergroundState : public MonsterStateMachine {
    public:
        static BossUndergroundState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        BossUndergroundState() {}
        float UndergroundTimer = 0.f;
    };

    // Spawn 상태
    class BossSpawnState : public MonsterStateMachine {
    public:
        static BossSpawnState& GetInstance();
        void Enter(Monster* monster) override;
        void Update(Monster* monster) override;
        void Exit(Monster* monster) override;
    private:
        BossSpawnState() {}
        float SpawnTimer;
    };

}

#endif