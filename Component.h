#pragma once
#include"stdafx.h"

enum class COMPONENT_TYPE : UINT8
{
	TRANSFORM,
	MESH_RENDERER,
	CAMERA,
	LIGHT,
	// ...
	MONO_BEHAVIOUR,
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
	std::weak_ptr<class GameObject> owner;

public:
	CComponent(COMPONENT_TYPE type);
	virtual ~CComponent();
public:
	COMPONENT_TYPE GetType() const { return type; }
};
