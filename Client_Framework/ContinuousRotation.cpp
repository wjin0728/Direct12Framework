#include "stdafx.h"
#include "ContinuousRotation.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "Camera.h"

CContinuousRotation::CContinuousRotation()
{
}

CContinuousRotation::CContinuousRotation(const CContinuousRotation& other) : CComponent(other)
{
	mRotationSpeed = other.mRotationSpeed;
	mRotationAxis = other.mRotationAxis;
}

CContinuousRotation::~CContinuousRotation()
{
}

void CContinuousRotation::Awake()
{
}

void CContinuousRotation::Start()
{
}

void CContinuousRotation::Update()
{
	float deltaTime = DELTA_TIME;
	Rotate(deltaTime);
}

void CContinuousRotation::LateUpdate()
{
}

std::shared_ptr<CComponent> CContinuousRotation::Clone()
{
	return std::make_shared<CContinuousRotation>(*this);
}

void CContinuousRotation::Rotate(float deltaTime)
{
	if (mRotationSpeed == Vec3(0.f, 0.f, 0.f)) return;
	auto transform = GetTransform();

	Vec3 objRotation = transform->GetLocalEulerAngles();
	Vec3 mRotationAngle = objRotation + mRotationSpeed * deltaTime;
	mRotationAngle.x = SimpleMath::NormalizeAngle360(mRotationAngle.x);
	mRotationAngle.y = SimpleMath::NormalizeAngle360(mRotationAngle.y);
	mRotationAngle.z = SimpleMath::NormalizeAngle360(mRotationAngle.z);

	Vec3 rotationAngle = mRotationAxis * mRotationAngle;
	Quaternion rotation = Quaternion::CreateFromYawPitchRoll(rotationAngle.y * degToRad, rotationAngle.x * degToRad, rotationAngle.z * degToRad);
	transform->SetLocalRotation(rotation);

}
