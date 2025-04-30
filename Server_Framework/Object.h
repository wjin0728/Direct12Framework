#pragma once
#include "stdafx.h"

class Object
{
public:
	Vec3 position;
	Vec3 velocity;
	Quaternion rotation;
	S_OBJECT_TYPE type;

	Object() : position(Vec3::Zero), velocity(Vec3::Zero), rotation(Quaternion::Identity), type(S_OBJECT_TYPE::S_PLAYER) {};
	~Object() {};

	void SetPosition(float x, float y, float z) { position = Vec3(x, y, z); };
	Vec3 GetPosition() { return position; };
	void SetVelocity(float x, float y, float z) { velocity = Vec3(x, y, z); };
	Vec3 GetVelocity() { return velocity; };
	// void SetRotation(float x, float y, float z) { rotation = Quaternion(x, y, z); };
	Quaternion GetRotation() { return rotation; };
};

