#pragma once
#include"MonoBehaviour.h"

class CFollowTarget : public CMonoBehaviour
{
private:
	std::shared_ptr<CGameObject> mTarget{};

public:
	CFollowTarget();
	CFollowTarget(const CFollowTarget& other) : CMonoBehaviour(other) {}
	virtual ~CFollowTarget();

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CFollowTarget>(*this); }

public:
	void SetTarget(const std::shared_ptr<CGameObject>& target) { mTarget = target; }
};

