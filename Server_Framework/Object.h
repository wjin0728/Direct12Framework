#pragma once
#include "stdafx.h"

class Object
{
public:
	Vec3 _pos;
	Vec3 _velocity;
	Vec3 _rotate;
	S_OBJECT_TYPE type;

	Object() : _pos(Vec3::Zero), _velocity(Vec3::Zero), _rotate(Vec3::Zero), type(S_OBJECT_TYPE::S_end) {};
	Object(S_OBJECT_TYPE obj_type) : _pos(Vec3::Zero), _velocity(Vec3::Zero), _rotate(Vec3::Zero), type(obj_type) {};
	~Object() {};

	void SetPosition(float x, float y, float z) { _pos = Vec3(x, y, z); };
	void SetVelocity(float x, float y, float z) { _velocity = Vec3(x, y, z); };
	void SetRotation(float x, float y, float z) { _rotate = Vec3(x, y, z); };
};

