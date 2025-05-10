#pragma once
#include "stdafx.h"
#include "Component.h"

class CItemMovement : public CComponent
{
private:
	float mAmplitude{ 0.f };
	float mFrequency{ 0.f };
	Vec3 mDirection{ 0.f, 0.f, 0.f };
	Vec3 mStartPos{ 0.f, 0.f, 0.f };

	std::shared_ptr<class CGameObject> mTargetObject{ nullptr };

public:
	CItemMovement() : CComponent(COMPONENT_TYPE::SCRIPT) {};
	CItemMovement(const CItemMovement& other) : CComponent(other), mAmplitude(other.mAmplitude), mFrequency(other.mFrequency), mDirection(other.mDirection) {};
	virtual ~CItemMovement() {};
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CItemMovement>(*this); };

public:
	void SetAmplitude(float amplitude) { mAmplitude = amplitude; }
	void SetFrequency(float frequency) { mFrequency = frequency; }
	void SetDirection(const Vec3& direction) { mDirection = direction; }
	void SetStartPos(const Vec3& startPos) { mStartPos = startPos; }
	void SetTargetObject(std::shared_ptr<class CGameObject> target) { mTargetObject = target; }
	Vec3 GetDirection() const { return mDirection; }
	Vec3 GetStartPos() const { return mStartPos; }

};