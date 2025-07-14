#pragma once
#include "Component.h"

class CTargetMarker :
    public CComponent
{
private:
	std::weak_ptr<class CUIRenderer> mRenderer;
	std::weak_ptr<class CGameObject> mTarget;

	bool mTargetChanged = false;
	bool mIsLocking = false;

	float mLockTime = 0.f;
	float mOriginalSize = 0.f;
	float mLockTimeMax = 0.2f;
	float mLockTimeRate;

public:
    CTargetMarker();
    virtual ~CTargetMarker();

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	void SetTarget(std::shared_ptr<CGameObject> target);

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CTargetMarker>(*this); }

};

