#pragma once
#include"stdafx.h"

enum class COMPONENT_TYPE : UINT8
{
	TRANSFORM,
	MESH_RENDERER,
	CAMERA,
	LIGHT,
	// ...
	SCRIPT,
	END,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<UINT8>(COMPONENT_TYPE::END) - 1
};

class CComponent
{
protected:
	COMPONENT_TYPE type;
	std::weak_ptr<class CGameObject> owner;

public:
	CComponent(COMPONENT_TYPE type);
	virtual ~CComponent();

	virtual void Awake() {}; 
	virtual void Start() {}; 

	virtual void Update() {}; 
	virtual void LateUpdate() {};
	virtual void FixedUpdate() {}; 

public:
	COMPONENT_TYPE GetType() const { return type; }
};

