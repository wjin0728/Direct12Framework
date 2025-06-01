#pragma once
#include "CEntityState.h"

class CEnemyState :
    public CEntityState
{
public:
    CEnemyState() : CEntityState((UINT8)MONSTER_STATE::IDLE) {}
    virtual ~CEnemyState() {}
    CEnemyState(const CEnemyState&) = delete;
	CEnemyState& operator=(const CEnemyState&) = delete;

public:
    virtual void Start() override {};
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;

	virtual ENEMY_TYPE GetEnemyType() const = 0;

};

class CGrassBigState :
    public CEnemyState
{
public:
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
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;

	virtual ENEMY_TYPE GetEnemyType() const override { return ENEMY_TYPE::GRASS_SMALL; };
};