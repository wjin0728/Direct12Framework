#include "stdafx.h"
#include "AvoidObstacle.h"
#include "RigidBody.h"
#include"GameObject.h"
#include"Transform.h"
#include"Timer.h"
#include"Collider.h"
#include"SceneManager.h"
#include"Scene.h"

CAvoidObstacle::CAvoidObstacle() : CMonoBehaviour("AvoidObstacle")
{
}

CAvoidObstacle::~CAvoidObstacle()
{
}

void CAvoidObstacle::Awake()
{
	mRigidBody = std::static_pointer_cast<CRigidBody>(GetOwner()->GetComponent(COMPONENT_TYPE::RIGID_BODY));
	destination = GetTransform()->GetWorldPosition() + Vec3::Backward * 100.f;
}

void CAvoidObstacle::Start()
{
}

void CAvoidObstacle::Update()
{
	auto transform = GetTransform();

	mRay.position = transform->GetWorldPosition();
	mRay.direction = transform->GetWorldLook().GetNormalized();

	if ((destination - mRay.position).Length() < 60.f) {
		destination += 50.f * Vec3::Backward;
	}

	CheckCollision();
}

void CAvoidObstacle::LateUpdate()
{
}

void CAvoidObstacle::CheckCollision()
{
	auto& objectMap = INSTANCE(CSceneManager).GetCurScene()->GetObjects();
	std::shared_ptr<CGameObject> nearestObj{};
	float minDist = FLT_MAX;

	for (const auto& [layer, objects] : objectMap) {
		for (const auto& object : objects) {
			float dist = IsCollided(object);
			if (!SimpleMath::IsZero(dist) && minDist > dist)
				nearestObj = object;
		}
	}
	auto transform = GetTransform();
	float turnPower = mRigidBody->GetVelocity().Length() / 20.f;

	if (!nearestObj) {
		Vec3 dir = (destination - transform->GetWorldPosition()).GetNormalized();
		Quaternion rotation = Quaternion::Slerp(transform->GetLocalRotation(), Quaternion::LookRotation(dir), DELTA_TIME * turnPower);
		transform->SetLocalRotation(rotation);
		isAvoiding = false;
		return;
	}

	if (!isAvoiding) {
		mLastDir = mRay.direction;
		isAvoiding = true;
		mRigidBody->SetUseFriction(false);
	}

	mRigidBody->SetUseFriction(true);
	mRigidBody->SetFriction(25.f);
	if (mRigidBody->GetVelocity().Length() < 10.f) mRigidBody->SetUseFriction(false);

	Vec3 obstaclePos = nearestObj->GetTransform()->GetWorldPosition();
	Vec3 position = transform->GetWorldPosition();

	Vec3 obstacleDir = (obstaclePos - position).GetNormalized();
	Vec3 avoidDir = mRay.direction.Cross(Vec3::Up).GetNormalized();
	if (avoidDir.Dot(obstacleDir) > 0) {
		avoidDir = -avoidDir;
	}

	Quaternion rotation = Quaternion::Slerp(transform->GetLocalRotation(), Quaternion::LookRotation(avoidDir), DELTA_TIME * turnPower);

	transform->SetLocalRotation(rotation);
}

float CAvoidObstacle::IsCollided(const std::shared_ptr<CGameObject>& obj)
{
	if (obj->GetTag() != L"Obstacle") {
		return 0.f;
	}
	if (!obj->GetCollider()) {
		return 0.f;
	}

	float distance{};
	mRay.Intersects(obj->GetCollider()->GetWorldOOBB(), distance);

	if (distance > 20.f) distance = 0.f;

	return distance;
}
