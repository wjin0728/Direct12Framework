#pragma once
#include "stdafx.h"

struct EventKey
{
	float mTime = 0.0f; // 이벤스 발생 시점
	std::string mName; // 이벤트 이름
	bool mEnable = true; // 활성화 여부 (기본은 활성화. 현재 시간이 이벤트 발생 시점보다 뒤이면 이벤트 발생시키고 비활성화. 애니메이션 초기화시 true로 돌아감)
	float mData = 0.0f;
};

struct AnimationInfo {
	string mAnimationName; // 애니메이션 이름

	float mLength = 0.0f; // 애니메이션 길이 (초 단위)
	int mFrameLength = 0; // 애니메이션 프레임 길이

	ANIMATION_TYPE mType = ANIMATION_TYPE::LOOP; //Once, Loop, PingPong, End
	vector<EventKey> mEventKeys; // 이벤트 키 정보
};

class CAnimationEventHandler
{
public:
	CAnimationEventHandler() {}
	CAnimationEventHandler(const CAnimationEventHandler& other) = default;
	~CAnimationEventHandler() {}

	using Event = std::function<void(class Monster*)>;

	void Register(const std::string& name, Event event) {
		mEvents[name] = std::move(event);
	}

	Event GetEvent(const std::string& name) const {
		auto it = mEvents.find(name);
		if (it != mEvents.end()) return it->second;
		return nullptr;
	}

	std::unordered_map<std::string, Event> mEvents;
};

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

