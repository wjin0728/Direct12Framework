#pragma once
#include "Object.h"
class Projectile :
    public Object
{
public:
	float _speed = 1.f; // 발사 속도
	bool _user_frinedly;
	S_PROJECTILE_TYPE _p_type = S_PROJECTILE_TYPE::PROJECTILE_END;

	Projectile() :
		Object(S_OBJECT_TYPE::S_PLAYER_PROJECTILE),
		_user_frinedly(1) {
	}
	Projectile(bool user_frinedly, S_PROJECTILE_TYPE type) :
		Object(S_OBJECT_TYPE::S_PLAYER_PROJECTILE),
		_user_frinedly(user_frinedly),
		_p_type(type) {
		switch (type)
		{
		case S_PROJECTILE_TYPE::ARROW:
			_boundingbox.Center = XMFLOAT3(0, 0, 0.2230943);
			_boundingbox.Extents = Vec3(0.106043, 0.03977784, 0.6090841) / 2.f;
			break;
		case S_PROJECTILE_TYPE::FIREBALL:
			break;
		case S_PROJECTILE_TYPE::ICEBALL:
			break;
		case S_PROJECTILE_TYPE::GRASSBALL:
			break;
		case S_PROJECTILE_TYPE::MAGIC_BALL:
			_boundingbox.Center = XMFLOAT3(0, 0, 0);
			_boundingbox.Extents = Vec3(0.8, 0.8, 0.8) / 2.f;
			break;
		case S_PROJECTILE_TYPE::PROJECTILE_END:
			break;
		default:
			break;
		}

	}
	void Update() {
		_pos += (_velocity * _speed);
	}
};

