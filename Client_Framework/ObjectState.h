#pragma once
#include "stdafx.h"
#include "Component.h"

class CObjectStateMachine : public CComponent
{
protected:
    PLAYER_STATE currentState;
    PLAYER_CLASS mClass = PLAYER_CLASS::ARCHER;

	std::weak_ptr<CAnimationController> mAnimationController{};

public:
    CObjectStateMachine() : currentState(PLAYER_STATE::IDLE) {}
    CObjectStateMachine(uint8_t playerClass) : currentState(PLAYER_STATE::IDLE) { mClass = (PLAYER_CLASS)playerClass; }
    CObjectStateMachine(const CObjectStateMachine& other) : CComponent(other) {}
    virtual ~CObjectStateMachine() = default;

    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CObjectStateMachine>(); }

    void SetState(PLAYER_STATE newState) {
        if (currentState != newState) {
            OnExitState(currentState);
            currentState = newState;
            OnEnterState(currentState);
        }
    }
    void SetClass(PLAYER_CLASS playerClass) { mClass = playerClass; };

    PLAYER_STATE GetState() const { return currentState; }
    PLAYER_CLASS GetClass() const { return mClass; }

protected:
    virtual void OnEnterState(PLAYER_STATE state);
    virtual void OnExitState(PLAYER_STATE state);
};
