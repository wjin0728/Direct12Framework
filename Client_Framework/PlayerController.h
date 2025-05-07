#pragma once
#include "stdafx.h"
#include "MonoBehaviour.h"

class CPlayerController : public CMonoBehaviour
{
private:
	PLAYER_STATE mState = PLAYER_STATE::IDLE;

	std::shared_ptr<class CAnimationController> mAnimationController{};
	std::shared_ptr<class CRigidBody> rigidBody{};
	std::weak_ptr<class CCamera> mCamera{};
	std::weak_ptr<class CTerrain> mTerrain{};

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
	void SetAnimationController(const std::shared_ptr<class CAnimationController>& animationController) { mAnimationController = animationController; }
	void SetChildAnimationController();
	void SetRigidBody(const std::shared_ptr<class CRigidBody>& rigidBody) { this->rigidBody = rigidBody; }
	void SetCamera(const std::shared_ptr<class CCamera>& camera) { mCamera = camera; }
	void SetTerrain(const std::shared_ptr<class CTerrain>& terrain) { mTerrain = terrain; }

	void SetState(PLAYER_STATE state);

	void OnKeyEvents();
};

