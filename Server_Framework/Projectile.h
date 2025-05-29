#pragma once
#include "Object.h"
class Projectile :
    public Object
{
public:
	float _speed = 1.f; // 발사 속도
	bool _user_frinedly;
	S_PROJECTILE_TYPE _type = S_PROJECTILE_TYPE::PROJECTILE_END;

	Projectile() :
		Object(S_OBJECT_TYPE::S_PLAYER_PROJECTILE),
		_user_frinedly(1) {
	}
	Projectile(bool user_frinedly, S_PROJECTILE_TYPE type) :
		Object(S_OBJECT_TYPE::S_PLAYER_PROJECTILE),
		_user_frinedly(user_frinedly),
		_type(type) {
	}
	void Update() {
		_pos += (_velocity * _speed);
	}
};

