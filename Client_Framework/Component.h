#pragma once
#include"GameObject.h"

class CTransform;

enum class COMPONENT_TYPE : UINT8
{
	TRANSFORM,
	COLLIDER,
	RENDERER,
	MESH_RENDERER,
	SKINNED_RENDERER,
	CAMERA,
	LIGHT,
	TERRAIN,
	RIGID_BODY,
	ANIMATION,
	// ...
	SCRIPT,

	NOTHING,
	END
};

class CComponent
{
protected:
	COMPONENT_TYPE type{};
	CGameObject* owner{};

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
	void SetOwner(CGameObject* _owner);

	CGameObject* GetOwner() const;
	std::shared_ptr<CTransform> GetTransform() const;
	COMPONENT_TYPE GetType() const { return type; }
};

