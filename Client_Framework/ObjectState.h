#pragma once
#include "stdafx.h"
#include "Component.h"
#include "CEntityState.h"

class CPlayerStateMachine : public CEntityState
{
protected:
    PLAYER_CLASS mClass = PLAYER_CLASS::ARCHER;
    std::weak_ptr<class CGameObject> mShield{};
	float mShieldDuration = -1.0f; // Duration for which the shield is active
	float mShieldDurationMax = 5.0f; // Maximum duration for the shield

public:
    CPlayerStateMachine() : CEntityState((UINT8)PLAYER_STATE::IDLE) {}
    CPlayerStateMachine(uint8_t playerClass) : CEntityState((UINT8)PLAYER_STATE::IDLE) { mClass = (PLAYER_CLASS)playerClass; }
    CPlayerStateMachine(const CPlayerStateMachine& other) : CEntityState(other) {}
    virtual ~CPlayerStateMachine() = default;
    virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CPlayerStateMachine>(); }

    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;

    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;

public:
    void SetClass(PLAYER_CLASS playerClass) { mClass = playerClass; };
    PLAYER_CLASS GetClass() const { return mClass; }
	void SetShield(std::weak_ptr<class CGameObject> shield) { mShield = shield; }
    void ActivateShield(bool activate)
    {
        if (auto shield = mShield.lock())
        {
            shield->SetActive(activate);
            if (activate)
            {
                mShieldDuration = mShieldDurationMax; // Reset shield duration when activated
			}
        }
	}
};

class CArcherState : public CPlayerStateMachine
{
private:

public:
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;

    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
};

class CWarriorState : public CPlayerStateMachine
{
private:
    public:
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
};