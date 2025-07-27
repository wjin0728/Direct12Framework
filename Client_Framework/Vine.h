#pragma once
#include"stdafx.h"
#include "Component.h"


class CVine :
	public CComponent
{
private:
	float mTotalDuration = 0.0f;
	float mTransitionDuration = 0.0f;

	float mTime = 0.0f;
	float mSpeed = 0.0f;
	float mOffset = 0.0f;

	float mMinPosY = 0.0f;
	float mMaxPosY = 0.0f;

	float mMinRotationY = 0.0f;
	float mMaxRotationY = 0.0f;

	Vec3 mMinScale = Vec3::Zero;
	Vec3 mMaxScale = Vec3::Zero;

public:
	CVine() {};
	virtual ~CVine() = default;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CVine>(*this); }

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
};