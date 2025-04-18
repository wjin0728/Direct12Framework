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

	if (mVelocity != Vec3::Zero) {
		GetTransform()->Move(mVelocity, DELTA_TIME);
	}

	if (mAcceleration != Vec3::Zero) {
		mVelocity += (mAcceleration * DELTA_TIME);
		mVelocity = Vec3::ClampLength(mVelocity, 0.f, mMaxVelocity);
	}
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

    float velocityLength = mVelocity.Length();
    if (velocityLength < 0.01f) {
        mVelocity = Vec3::Zero;
        mUseFriction = false;
        return;
    }

    Vec3 decelerateDir = -mVelocity / velocityLength;
    float deceleration = mFriction * DELTA_TIME;
    if (deceleration > velocityLength) {
        deceleration = velocityLength;
    }
    mVelocity += decelerateDir * deceleration;
}

void CRigidBody::SetUseFriction(bool use)
{
	mUseFriction = use;
}

