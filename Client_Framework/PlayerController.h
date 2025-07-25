#pragma once
#include "stdafx.h"
#include "Component.h"
#include "CutScene.h"

class CPlayerController : public CComponent
{
public:
	enum class ControllMode
	{
		None,
		FreeLook, // Free look mode, camera follows the player
		LockOn,   // Lock on mode, camera focuses on a target
		ClassSelection // Class selection mode, camera is static
	};
private:
	ControllMode mControllMode = ControllMode::None;
	ITEM_TYPE mSkill = ITEM_TYPE::item_end;
	PLAYER_CLASS mClass = PLAYER_CLASS::ARCHER;

	std::shared_ptr<class CPlayerStateMachine> mStateMachine{};
	std::shared_ptr<class CCutScene> mCutScene{};
	std::weak_ptr<class CCamera> mCamera{};
	std::weak_ptr<class CTerrain> mTerrain{};

	std::weak_ptr<class CGameObject> mTargetEnemy{};
	std::weak_ptr<class CGameObject> mTargetItem{};

	
	bool mActive = true;
	bool moveKeyPressed = false;
	bool mFreeLook = false;
	bool mCastingSkill = false;
	float mMaxAttackRange = 9.0f;

public:
	CPlayerController() : CComponent() {};
	CPlayerController(const CPlayerController& other) : CComponent(other) {}
	virtual ~CPlayerController();

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CPlayerController>(*this); }

public:
	ITEM_TYPE GetSkill() const { return mSkill; }

	void SetStateMachine(const std::shared_ptr<class CPlayerStateMachine>& StateMachine) { mStateMachine = StateMachine; }
	void SetChildAnimationController();
	void SetCamera(const std::shared_ptr<class CCamera>& camera) { mCamera = camera; }
	void SetTerrain(const std::shared_ptr<class CTerrain>& terrain) { mTerrain = terrain; }
	void SetSkill(ITEM_TYPE skill);
	void SetClass(PLAYER_CLASS playerClass) { mClass = playerClass; };
	void SetCutScene(const std::shared_ptr<class CCutScene>& cutScene) { mCutScene = cutScene; }

	void SetState(PLAYER_STATE state);

	void OnKeyEvents();

	void SetActive(bool active) { mActive = active; };
	void ChangeControllMode(ControllMode mode);
private:
	void CastingSkill();
	void LockOnTarget();
	void InteractWithItem();

};

