#pragma once
#include"GameObject.h"

class CTransform;

class CComponent
{
protected:
	CGameObject* owner{};

public:
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

	CGameObject* GetOwner() const;
	std::shared_ptr<CTransform> GetTransform() const;
};

