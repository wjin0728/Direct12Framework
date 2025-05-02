#pragma once
#include "Object.h"
class PlayerCharacter : public Object
{
public:
	Vec3				_look_dir; // 캐릭터가 보고있는 방향
	Vec3				_acceleration; // 추가: 가속도
	uint8_t			    _class;

	PlayerCharacter() :
		Object(S_OBJECT_TYPE::S_PLAYER),
		_look_dir(Vec3::Zero), 
		_acceleration(Vec3::Zero), 
		_class(0) {};
	~PlayerCharacter() {};

	void SetLookDir(float x, float y, float z) { _look_dir = Vec3(x, y, z); };
	void SetAcceleration(float x, float y, float z) { _acceleration = Vec3(x, y, z); };
	void SetClass(uint8_t player_class) { _class = player_class; };
};

