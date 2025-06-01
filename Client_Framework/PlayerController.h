#pragma once
#include "stdafx.h"
#include "MonoBehaviour.h"

class CPlayerController : public CMonoBehaviour
{
private:
	ITEM_TYPE mSkill = ITEM_TYPE::item_end;
	PLAYER_CLASS mClass = PLAYER_CLASS::ARCHER;

	std::shared_ptr<class CPlayerStateMachine> mStateMachine{};
	std::shared_ptr<class CRigidBody> rigidBody{};
	std::weak_ptr<class CCamera> mCamera{};
	std::weak_ptr<class CTerrain> mTerrain{};

	bool moveKeyPressed = false;
	bool mFreeLook = false;

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

	void SetStateMachine(const std::shared_ptr<class CPlayerStateMachine>& StateMachine) { mStateMachine = StateMachine; }
	void SetChildAnimationController();
	void SetRigidBody(const std::shared_ptr<class CRigidBody>& rigidBody) { this->rigidBody = rigidBody; }
	void SetCamera(const std::shared_ptr<class CCamera>& camera) { mCamera = camera; }
	void SetTerrain(const std::shared_ptr<class CTerrain>& terrain) { mTerrain = terrain; }
	void SetSkill(ITEM_TYPE skill) { mSkill = skill; };
	void SetClass(PLAYER_CLASS playerClass) { mClass = playerClass; };

	void SetState(PLAYER_STATE state);

	void OnKeyEvents();

private:

};

