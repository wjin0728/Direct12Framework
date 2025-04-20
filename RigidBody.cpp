#include "stdafx.h"
#include "RigidBody.h"
#include"GameObject.h"
#include"Transform.h"
#include"Timer.h"
#include"Terrain.h"
#include"SceneManager.h"
#include"Scene.h"

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
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	mTerrain = scene->GetTerrain();
}

void CRigidBody::Update()
{
	if (mVelocity != Vec3::Zero) {
		Vec3 pos = GetValidGroundPosition();
		GetTransform()->SetLocalPosition(pos);
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

Vec3 CRigidBody::GetValidGroundPosition()
{
	auto transform = GetTransform();
	Vec3 pos = transform->GetWorldPosition();
	Vec3 prevPos = pos + mVelocity * DELTA_TIME;

	auto terrain = mTerrain.lock();
	if (terrain && terrain->CanMove(prevPos.x, prevPos.z)) return prevPos;

	Vec3 moveDir = mVelocity.GetNormalized();
    for (int angle = 1; angle <= 90; angle += 5)
    {
		float rad = angle * degToRad;
        Vec3 rightCheck = Quaternion::CreateFromYawPitchRoll(rad, 0, 0) * mVelocity;
        Vec3 leftCheck = Quaternion::CreateFromYawPitchRoll(-rad, 0, 0) * mVelocity;
        Vec3 rPos = pos + rightCheck * DELTA_TIME;
		Vec3 lPos = pos + leftCheck * DELTA_TIME;

        if (terrain->CanMove(rPos.x, rPos.z)) return rPos;
        if (terrain->CanMove(lPos.x, lPos.z)) return lPos;
    }

	return pos;
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

