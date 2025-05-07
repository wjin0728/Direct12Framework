#pragma once
#include <DirectXMath.h>
using namespace DirectX;

// 전방 선언
class Monster;

// 상태 머신의 추상 기본 클래스
class MonsterState {
public:
    virtual ~MonsterState() {}
    virtual void Enter(Monster* monster) = 0;
    virtual void Update(Monster* monster) = 0;
    virtual void Exit(Monster* monster) = 0;
};

// Idle 상태
class IdleState : public MonsterState {
public:
    static IdleState& GetInstance();
    void Enter(Monster* monster) override;
    void Update(Monster* monster) override;
    void Exit(Monster* monster) override;
private:
    IdleState() {}
};

// Run 상태
class RunState : public MonsterState {
public:
    static RunState& GetInstance();
    void Enter(Monster* monster) override;
    void Update(Monster* monster) override;
    void Exit(Monster* monster) override;
private:
    RunState() {}
};

// BasicAttack 상태
class BasicAttackState : public MonsterState {
public:
    static BasicAttackState& GetInstance();
    void Enter(Monster* monster) override;
    void Update(Monster* monster) override;
    void Exit(Monster* monster) override;
private:
    BasicAttackState() {}
    float attackTimer;
};

// SkillAttack 상태
class SkillAttackState : public MonsterState {
public:
    static SkillAttackState& GetInstance();
    void Enter(Monster* monster) override;
    void Update(Monster* monster) override;
    void Exit(Monster* monster) override;
private:
    SkillAttackState() {}
    float skillTimer;
};

// Hit 상태
class HitState : public MonsterState {
public:
    static HitState& GetInstance();
    void Enter(Monster* monster) override;
    void Update(Monster* monster) override;
    void Exit(Monster* monster) override;
private:
    HitState() {}
    float hitTimer;
};