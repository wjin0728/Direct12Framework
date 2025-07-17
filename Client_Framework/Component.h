#pragma once
#include"GameObject.h"

class CTransform;

class CComponent
{
protected:
	CGameObject* owner{};

public:
	bool mIsActive{ true };
	bool mIsAwake{ false };
	bool mIsStart{ false };
	CComponent();
	CComponent(const CComponent& other);
	virtual ~CComponent();

	virtual void Awake() {}; 
	virtual void Start() {}; 

	virtual void Update() {}; 
	virtual void LateUpdate() {};

	virtual std::shared_ptr<CComponent> Clone() = 0;

public:
	void SetOwner(CGameObject* _owner);
	void EnqueueAwake();

	CGameObject* GetOwner() const;
	std::shared_ptr<CTransform> GetTransform() const;
};

