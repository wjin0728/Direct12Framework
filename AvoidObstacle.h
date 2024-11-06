#pragma once
#include "MonoBehaviour.h"

class CAvoidObstacle : public CMonoBehaviour
{
private:
	Ray mRay{};
	float mMaxDistance = 50.f;
	std::shared_ptr<class CRigidBody> mRigidBody{};

	bool isAvoiding{};
	Vec3 mLastDir = Vec3::Backward;
	Vec3 destination{};

public:
	CAvoidObstacle();
	~CAvoidObstacle();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CAvoidObstacle>(); }

public:
	void CheckCollision();
	float IsCollided(const std::shared_ptr<CGameObject>& obj);
};

