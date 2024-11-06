#include "stdafx.h"
#include "RigidBody.h"
#include"GameObject.h"
#include"Transform.h"
#include"Timer.h"

CRigidBody::CRigidBody() : CComponent(COMPONENT_TYPE::RIGID_BODY)
{

}

CRigidBody::~CRigidBody()
{
}

void CRigidBody::Awake()
{
}

void CRigidBody::Start()
{
}

void CRigidBody::Update()
{
	if (mAcceleration != Vec3::Zero) {
		mVelocity += (mAcceleration * DELTA_TIME);
		mVelocity = Vec3::ClampLength(mVelocity, 0.f, mMaxVelocity);
	}

	GetTransform()->Move(mVelocity, DELTA_TIME);
	Decelerate();
}

void CRigidBody::LateUpdate()
{
}

void CRigidBody::Decelerate()
{
	if (!mUseFriction) {
		return;
	}

	Vec3 decelerateDir = -mVelocity.GetNormalized();
	float deceleration = mFriction * DELTA_TIME;

	if (deceleration > mVelocity.Length()) {
		deceleration = mVelocity.Length();
	}

	mVelocity += decelerateDir * deceleration;
	if (mVelocity.Length() < 0.01f) {
		mVelocity = Vec3::Zero;
		mUseFriction = false;
	}
}
