#pragma once
#include "stdafx.h"

class Object
{
public:
	Vec3					_pos;
	Vec3					_velocity;
	Quaternion				_rotation;
	BoundingOrientedBox 	_orignalboundingbox;
	BoundingOrientedBox 	_boundingbox;
	S_OBJECT_TYPE			_type;
	bool					_active = true;

	Object() : _pos(Vec3::Zero), 
		_velocity(Vec3::Zero),
		_rotation(Quaternion::Identity), 
		_type(S_OBJECT_TYPE::S_end) {};

	Object(S_OBJECT_TYPE obj_type);
	~Object() {};

	void SetPosition(float x, float y, float z) { _pos = Vec3(x, y, z); };
	void SetVelocity(float x, float y, float z) { _velocity = Vec3(x, y, z); };
	void LocalTransform() {
		Matrix mLocalMat = Matrix::CreateFromQuaternion(_rotation);
		mLocalMat._41 = _pos.x;
		mLocalMat._42 = _pos.y;
		mLocalMat._43 = _pos.z;
		_orignalboundingbox.Transform(_boundingbox, mLocalMat);
	};
};

