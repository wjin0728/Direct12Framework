#pragma once
#include "Component.h"

class CContinuousRotation : public CComponent
{
private:
	Vec3 mRotationSpeed{ 0.f, 0.f, 0.f };
	Vec3 mRotationAxis{ 0.f, 1.f, 0.f };
public:
	CContinuousRotation();
	CContinuousRotation(const CContinuousRotation& other);
	~CContinuousRotation();
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;
	std::shared_ptr<CComponent> Clone() override;

public:
	void SetRotationSpeed(const Vec3& speed) { mRotationSpeed = speed; }
	void SetRotationAxis(const Vec3& axis) { mRotationAxis = axis; }
	Vec3 GetRotationSpeed() const { return mRotationSpeed; }
	Vec3 GetRotationAxis() const { return mRotationAxis; }

	void Rotate(float deltaTime);
};

