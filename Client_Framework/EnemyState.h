#pragma once
#include "CEntityState.h"

class CGrassBigState :
    public CEntityState
{
public:
    virtual void Start() override;
    virtual void Update() override;

    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
};

class CGrassSmallState :
    public CEntityState
{
    public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
};