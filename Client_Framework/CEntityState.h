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

	int mHealth{};
	int mMaxHealth{};

public:
    CEntityState() = default;
	CEntityState(UINT8 currentState) : currentState(currentState) {}
    ~CEntityState() = default;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CEntityState>(); }

public:
	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;

public:
	void SetState(UINT8 newState) {
		if (currentState != newState) {
			OnExitState(currentState);
			currentState = newState;
			OnEnterState(currentState);
		}
	}
	UINT8 GetState() const { return currentState; }

protected:
	virtual void OnEnterState(UINT8 state) = 0;
	virtual void OnExitState(UINT8 state) = 0;
};

