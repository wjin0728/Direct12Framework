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

	Object() : _pos(Vec3::Zero), 
		_velocity(Vec3::Zero),
		_rotation(Quaternion::Identity), 
		_type(S_OBJECT_TYPE::S_end) {};

	Object(S_OBJECT_TYPE obj_type) : _pos(Vec3::Zero),
		_velocity(Vec3::Zero),
		_rotation(Quaternion::Identity),
		_type(obj_type) 
	{
		if (obj_type == S_OBJECT_TYPE::S_PLAYER) {
			_boundingbox.Center = XMFLOAT3(0.01488304f, 0.5898448f, -0.01438522f);
			_boundingbox.Extents = XMFLOAT3(0.6752968f, 1.013497f, 0.6468925f);
			_orignalboundingbox.Center = XMFLOAT3(0.01488304f, 0.5898448f, -0.01438522f);
			_orignalboundingbox.Extents = XMFLOAT3(0.6752968f, 1.013497f, 0.6468925f);
		}
	};
	~Object() {};

	void SetPosition(float x, float y, float z) { _pos = Vec3(x, y, z); };
	void SetVelocity(float x, float y, float z) { _velocity = Vec3(x, y, z); };
	void LocalTransform() {
		Matrix mLocalMat = Matrix::CreateFromQuaternion(_rotation);
		mLocalMat._41 = _pos.x;
		mLocalMat._42 = _pos.y;
		mLocalMat._43 = _pos.z;
		_boundingbox.Transform(_boundingbox, mLocalMat);
	};
};

