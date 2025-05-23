#pragma once
#include"stdafx.h"
#include"Component.h"

class CRigidBody : public CComponent
{
private:
	bool mUseGravity{};
	bool mUseFriction{};
	Vec3 mVelocity{};
	Vec3 mAcceleration{};

	float mMaxVelocity = 2.f;
	float mFriction = 100.0f;

	std::weak_ptr<class CTerrain> mTerrain;

public:
	CRigidBody();
	~CRigidBody();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CRigidBody>(); }

private:
	Vec3 GetValidGroundPosition();
	void Decelerate();

public:

public:
	const Vec3& GetVelocity() const { return mVelocity; }
	bool GetUseGravity() const { return mUseGravity; }
	float GetMaxVelocity() const { return mMaxVelocity; }
	float GetFriction() const { return mFriction; }
	const Vec3& GetAcceleraion() const { return mAcceleration; }

	void SetVelocity(const Vec3& vel)  { mVelocity = vel; }
	void SetUseGravity(bool use) { mUseGravity = use; }
	void SetUseFriction(bool use);
	void SetMaxVelocity(float maxVel) { mMaxVelocity = maxVel; }
	void SetFriction(float friction) { mFriction = friction; }
	void SetAcceleration(const Vec3& accel) { mAcceleration = accel; }
};

