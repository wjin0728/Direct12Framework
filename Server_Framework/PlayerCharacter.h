#pragma once
#include "Object.h"
class PlayerCharacter : public Object
{
public:
	Vec3				_look_dir; // ĳ���Ͱ� �����ִ� ����
	Vec3				_acceleration; // �߰�: ���ӵ�
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

