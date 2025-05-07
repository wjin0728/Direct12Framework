#pragma once
#include <DirectXMath.h>
using namespace DirectX;

// ���� ����
class Monster;

// ���� �ӽ��� �߻� �⺻ Ŭ����
class MonsterState {
public:
    virtual ~MonsterState() {}
    virtual void Enter(Monster* monster) = 0;
    virtual void Update(Monster* monster) = 0;
    virtual void Exit(Monster* monster) = 0;
};

// Idle ����
class IdleState : public MonsterState {
public:
    static IdleState& GetInstance();
    void Enter(Monster* monster) override;
    void Update(Monster* monster) override;
    void Exit(Monster* monster) override;
private:
    IdleState() {}
};

// Run ����
class RunState : public MonsterState {
public:
    static RunState& GetInstance();
    void Enter(Monster* monster) override;
    void Update(Monster* monster) override;
    void Exit(Monster* monster) override;
private:
    RunState() {}
};

// BasicAttack ����
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

// SkillAttack ����
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

// Hit ����
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