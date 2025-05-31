#pragma once
#include "stdafx.h"
#include "Component.h"
#include "CEntityState.h"

class CObjectStateMachine : public CEntityState
{
protected:
    PLAYER_CLASS mClass = PLAYER_CLASS::ARCHER;

public:
    CObjectStateMachine() : CEntityState((UINT8)PLAYER_STATE::IDLE) {}
    CObjectStateMachine(uint8_t playerClass) : CEntityState((UINT8)PLAYER_STATE::IDLE) { mClass = (PLAYER_CLASS)playerClass; }
    CObjectStateMachine(const CObjectStateMachine& other) : CEntityState(other) {}
    virtual ~CObjectStateMachine() = default;

    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CObjectStateMachine>(); }

    void SetClass(PLAYER_CLASS playerClass) { mClass = playerClass; };

    
    PLAYER_CLASS GetClass() const { return mClass; }

    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
};
