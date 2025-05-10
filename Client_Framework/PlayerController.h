#pragma once
#include "stdafx.h"
#include "MonoBehaviour.h"

class CPlayerController : public CMonoBehaviour
{
private:
	PLAYER_STATE mState = PLAYER_STATE::IDLE;
	ITEM_TYPE mSkill = ITEM_TYPE::item_end;
	PLAYER_CLASS mClass = PLAYER_CLASS::ARCHER;

	std::shared_ptr<class CAnimationController> mAnimationController{};
	std::shared_ptr<class CRigidBody> rigidBody{};
	std::weak_ptr<class CCamera> mCamera{};
	std::weak_ptr<class CTerrain> mTerrain{};

	bool moveKeyPressed = false;

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
	uint8_t GetAnimationIndexFromState(PLAYER_STATE state);

	void SetAnimationController(const std::shared_ptr<class CAnimationController>& animationController) { mAnimationController = animationController; }
	void SetChildAnimationController();
	void SetRigidBody(const std::shared_ptr<class CRigidBody>& rigidBody) { this->rigidBody = rigidBody; }
	void SetCamera(const std::shared_ptr<class CCamera>& camera) { mCamera = camera; }
	void SetTerrain(const std::shared_ptr<class CTerrain>& terrain) { mTerrain = terrain; }
	void SetSkill(ITEM_TYPE skill) { mSkill = skill; };
	void SetClass(PLAYER_CLASS playerClass) { mClass = playerClass; };

	void SetState(PLAYER_STATE state);

	void OnKeyEvents();
};

