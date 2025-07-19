#pragma once
#include "stdafx.h"
#include "Component.h"

class CEntityState :
    public CComponent
{
protected:
	UINT8 currentState = 0;
	UINT8 nextState = 0;
	std::weak_ptr<class CAnimationController> mAnimationController{};
	std::weak_ptr<class CHealthSystem> mHealthSystem{};
	

public:
	int mHealth{};
	int mMaxHealth{};
	bool mIsHit = false;
	bool mIsDead = false;
	bool mIsSpawning = false;
	bool mIsSpawningFinished = false;
    CEntityState() = default;
	CEntityState(UINT8 currentState) : currentState(currentState) {}
    ~CEntityState() = default;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CEntityState>(*this); }

public:
	virtual void Awake(){};
	virtual void Start() {};
	virtual void Update() {};
	virtual void LateUpdate() {};

public:
	void SetState(UINT8 newState) {
		if (currentState != newState) {
			OnExitState(currentState);
			currentState = newState;
			OnEnterState(currentState);
		}
	}
	UINT8 GetState() const { return currentState; }
	std::weak_ptr<CAnimationController> GetAnimationController() const { return mAnimationController; }

protected:
	virtual void OnEnterState(UINT8 state) {};
	virtual void OnExitState(UINT8 state) {};

public:
	virtual void GetHit(float damage);
	virtual void Heal(float amount);
};

