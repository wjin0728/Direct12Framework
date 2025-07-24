#pragma once
#include "CEntityState.h"

class CEnemyState :
    public CEntityState
{
protected:
	static const float MAX_HEALTH;
    std::weak_ptr<class CTransform> mAttackSocket;
	float mHitProgress = -ANIMATION_CALLBACK_EPSILON;

public:
    CEnemyState() : CEntityState((UINT8)MONSTER_STATE::IDLE) {}
    virtual ~CEnemyState() {}
    CEnemyState(const CEnemyState&) = delete;
	CEnemyState& operator=(const CEnemyState&) = delete;

public:
	virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;

	virtual ENEMY_TYPE GetEnemyType() const = 0;

    void SetHitFactor(float hitFactor);
};

class CGrassBigState :
    public CEnemyState
{
public:
	virtual ~CGrassBigState() {}
	virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;

    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;


    virtual ENEMY_TYPE GetEnemyType() const override { return ENEMY_TYPE::GRASS_BIG; };
};

class CGrassSmallState :
    public CEnemyState
{
public:
	virtual ~CGrassSmallState() {}
	virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;

	virtual ENEMY_TYPE GetEnemyType() const override { return ENEMY_TYPE::GRASS_SMALL; };
};

class CWaterBigState :
    public CEnemyState
{
    public:
    virtual ~CWaterBigState() {}
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
    virtual ENEMY_TYPE GetEnemyType() const override { return ENEMY_TYPE::WATER_BIG; };
};

class CWaterSmallState :
    public CEnemyState
{
    public:
    virtual ~CWaterSmallState() {}
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
    virtual ENEMY_TYPE GetEnemyType() const override { return ENEMY_TYPE::WATER_SMALL; };
};

class CFireBigState :
    public CEnemyState
{
    public:
    virtual ~CFireBigState() {}
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
    virtual ENEMY_TYPE GetEnemyType() const override { return ENEMY_TYPE::FIRE_BIG; };
};


class CFireSmallState :
    public CEnemyState
{
    public:
    virtual ~CFireSmallState() {}
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
    virtual ENEMY_TYPE GetEnemyType() const override { return ENEMY_TYPE::FIRE_SMALL; };
};