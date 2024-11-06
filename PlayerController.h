#pragma once
#include "MonoBehaviour.h"

class CPlayerController : public CMonoBehaviour
{
private:
	float mTurnForce = 3.f;
	float mForwardForce = 10.f;
	float mForwardTiltForce = 20.f;

	Vec2 movement = Vec2::Zero;

	std::shared_ptr<class CRigidBody> rigidBody{};

	std::shared_ptr<CTransform> mainRotor{};
	std::shared_ptr<CTransform> subRotor{};

public:
	CPlayerController() : CMonoBehaviour("PlayerController") {};
	CPlayerController(const CPlayerController& other) : CMonoBehaviour(other) {}
	virtual ~CPlayerController();

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CPlayerController>(*this); }

public:
	void OnKeyEvents();
	void OnMouseEvents();
};

