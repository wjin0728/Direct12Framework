#pragma once

class CTransform;


#include"GameObject.h"

class CComponent
{
protected:
	COMPONENT_TYPE type{};
	std::weak_ptr<CGameObject> owner{};

public:
	CComponent(COMPONENT_TYPE type);
	CComponent(const CComponent& other);
	virtual ~CComponent();

	virtual void Awake() {}; 
	virtual void Start() {}; 

	virtual void Update() {}; 
	virtual void LateUpdate() {};

	virtual std::shared_ptr<CComponent> Clone() = 0;

public:
	void SetOwner(const std::shared_ptr<CGameObject>& _owner);

	std::shared_ptr<CGameObject> GetOwner(); const
	std::shared_ptr<CTransform> GetTransform(); const
	COMPONENT_TYPE GetType() const { return type; }
};

