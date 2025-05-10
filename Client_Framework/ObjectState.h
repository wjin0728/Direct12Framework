#pragma once
#include "stdafx.h"
#include "Component.h"

class CObjectStateMachine : public CComponent
{
protected:
    PLAYER_STATE currentState;

public:
    CObjectStateMachine() : CComponent(COMPONENT_TYPE::STATE), currentState(PLAYER_STATE::IDLE) {}
    CObjectStateMachine(const CObjectStateMachine& other) : CComponent(other) {}
    virtual ~CObjectStateMachine() = default;

    virtual void Awake() override {}
    virtual void Update() override;
    virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CObjectStateMachine>(); }

    void SetState(PLAYER_STATE newState) {
        if (currentState != newState) {
            OnExitState(currentState);
            currentState = newState;
            OnEnterState(currentState);
        }
    }

    PLAYER_STATE GetState() const { return currentState; }

protected:
    virtual void OnEnterState(PLAYER_STATE state);
    virtual void OnExitState(PLAYER_STATE state);
};
