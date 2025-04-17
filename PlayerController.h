#pragma once
#include "MonoBehaviour.h"

class CPlayerController : public CMonoBehaviour
{
private:

	std::shared_ptr<class CRigidBody> rigidBody{};
	std::weak_ptr<class CCamera> mCamera{};
	std::shared_ptr<class CTerrain> mTerrain{};

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
	void SetCamera(const std::shared_ptr<class CCamera>& camera) { mCamera = camera; }
	void SetTerrain(const std::shared_ptr<class CTerrain>& terrain) { mTerrain = terrain; }

	void OnKeyEvents();
};

