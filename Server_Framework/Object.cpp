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

void Object::ReadAnimationInfo(const std::string& fileName)
{
	using namespace BinaryReader;

	std::ifstream ifs{ fileName, std::ios::binary };
	if (!ifs) {
		return;
	}

	std::string token{};
	int setsNum{};

	while (true) {
		ReadDateFromFile(ifs, token);

		if (token == "<AnimationSets>:") {
			ReadDateFromFile(ifs, setsNum);

			_animations.resize(setsNum);
			for (auto& anim : _animations) {
				anim = std::make_shared<AnimationInfo>();
			}
		}
		else if (token == "<AnimationSet>:") {
			int setNum{}, framesPerSecondNum{}, keyFrameNum{};
			float length{};
			std::string setName;

			ReadDateFromFile(ifs, setNum);
			ReadDateFromFile(ifs, setName);
			ReadDateFromFile(ifs, length);
			ReadDateFromFile(ifs, framesPerSecondNum);
			ReadDateFromFile(ifs, keyFrameNum);

			auto animSet = _animations[setNum];
			animSet->mAnimationName = setName;
			animSet->mLength = length;
			animSet->mFrameLength = keyFrameNum;

			ReadDateFromFile(ifs, token);
			if (token == "<Loop>:") {
				int animationType{};
				ReadDateFromFile(ifs, animationType);
				animSet->mType = (ANIMATION_TYPE)animationType;
			}

			ReadDateFromFile(ifs, token);
			if (token == "<Events>:") {
				int eventCount{};
				ReadDateFromFile(ifs, eventCount);
				animSet->mEventKeys.resize(eventCount);

				for (auto& key : animSet->mEventKeys) {
					float eventTime{};
					std::string eventStr;

					ReadDateFromFile(ifs, eventTime);
					ReadDateFromFile(ifs, eventStr);

					key = std::make_shared<EventKey>(eventTime, eventStr);
				}
			}
		}
		else if (token == "</AnimationSets>")
		{
			break;
		}
	}

}