#include "Object.h"

Object::Object(S_OBJECT_TYPE obj_type) : _pos(Vec3::Zero),
_velocity(Vec3::Zero),
_rotation(Quaternion::Identity),
_type(obj_type)
{
	if (obj_type == S_OBJECT_TYPE::S_PLAYER) {
		_boundingbox.Center = XMFLOAT3(-0.002932072, 0.5853394, -0.02864045);
		_boundingbox.Extents = Vec3(0.6801331, 1.110679, 0.5878519) / 2.f;
		_orignalboundingbox = _boundingbox;
	}
}