#pragma once
#include "stdafx.h"
#include "Component.h"
#include "CEntityState.h"

class CPlayerStateMachine : public CEntityState
{
protected:
    PLAYER_CLASS mClass = PLAYER_CLASS::ARCHER;
    ElementType mElementType = ElementType::end;
    std::weak_ptr<class CGameObject> mShield{};
	int mShieldHealth = 0; // Health of the shield
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
	void SetElementType(ElementType elementType) { mElementType = elementType; }
    ElementType GetElementType() const { return mElementType; }
    void SetClass(PLAYER_CLASS playerClass) { mClass = playerClass; };
    PLAYER_CLASS GetClass() const { return mClass; }
	void SetShield(std::weak_ptr<class CGameObject> shield) { mShield = shield; }
	void SetShieldHealth(int health) { mShieldHealth = health; }
    void ActivateShield(bool activate)
    {
        if (auto shield = mShield.lock()) {
            shield->SetActive(activate);

            if (activate) {
                mShieldHealth = 3;
                mShieldDuration = mShieldDurationMax; // Reset shield duration when activated
			}
            else {
                mShieldHealth = 0;
                mShieldDuration = -1.f;
            }
        }
	}

    virtual void GetHit(float damage) override;
    virtual void Heal(float amount) override;
    virtual void UpdateHealth(float newHealth, int newSheild);
};

class CArcherState : public CPlayerStateMachine
{
private:

public:
	CArcherState() : CPlayerStateMachine((uint8_t)PLAYER_CLASS::ARCHER) {}
	virtual ~CArcherState() = default;

    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;

    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
};

class CWarriorState : public CPlayerStateMachine
{
private:
	std::weak_ptr<class CTransform> mAttackSocket;
	std::weak_ptr<class CTrailRenderer> mTrail;
public:
	CWarriorState() : CPlayerStateMachine((uint8_t)PLAYER_CLASS::FIGHTER) {}
	virtual ~CWarriorState() = default;
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
};

class CMageState : public CPlayerStateMachine
{
private:
public:
    std::weak_ptr<class CTransform> mAttackSocket;
public:
	CMageState() : CPlayerStateMachine((uint8_t)PLAYER_CLASS::MAGE) {}
	virtual ~CMageState() = default;
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnEnterState(UINT8 state) override;
    virtual void OnExitState(UINT8 state) override;
};
