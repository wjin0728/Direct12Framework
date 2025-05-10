#pragma once
#include "stdafx.h"
#include "Component.h"

template<typename T_STATE>
class CObjectStateMachine : public CComponent
{
protected:
    T_STATE currentState;

public:
    CObjectStateMachine() : CComponent(COMPONENT_TYPE::STATE), currentState(T_STATE::IDLE) {}
    CObjectStateMachine(const CObjectStateMachine& other) : CComponent(other) {}
    virtual ~CObjectStateMachine() = default;

    virtual void Awake() override {}
    virtual void Update() override {}
    virtual std::shared_ptr<CComponent> Clone() override {}

    virtual void UpdateState(float deltaTime, T_STATE state) {}

    void SetState(T_STATE newState) {
        if (currentState != newState) {
            OnExitState(currentState);
            currentState = newState;
            OnEnterState(currentState);
        }
    }

    T_STATE GetState() const { return currentState; }

protected:
    virtual void OnEnterState(T_STATE state) {}
    virtual void OnExitState(T_STATE state) {}
};

class PlayerStateMachine : public CObjectStateMachine<PLAYER_STATE>
{
protected:
    std::shared_ptr<CComponent> Clone() override { return std::make_shared<PlayerStateMachine>(); }

    void UpdateState(float deltaTime, PLAYER_STATE state) override;

    void OnEnterState(PLAYER_STATE state) override;
    void OnExitState(PLAYER_STATE state) override;
};
