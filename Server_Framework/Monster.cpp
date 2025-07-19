#include "Monster.h"
#include "PlayerCharacter.h"

void Monster::SetState(MonsterStateMachine* newState)
{
    if (currentState) {
        if (newState != &MonsterState::HitState::GetInstance()) {
            previousState = _state; 
            // MonsterState::HitState로 갈 때는 현재 상태 저장
        }
        currentState->Exit(this);
    }
    currentState = newState;
    if (currentState) currentState->Enter(this);
}

void Monster::SetState(S_MONSTER_STATE newState)
{
    switch (newState) {
    case S_MONSTER_STATE::IDLE:
        _state = S_MONSTER_STATE::IDLE;
        SetState(&MonsterState::IdleState::GetInstance());
        break;
    case S_MONSTER_STATE::RUN:
        _state = S_MONSTER_STATE::RUN;
        SetState(&MonsterState::RunState::GetInstance());
        break;
    case S_MONSTER_STATE::ATTACK:
        _state = S_MONSTER_STATE::ATTACK;
        SetState(&MonsterState::AttackState::GetInstance());
        break;
	case S_MONSTER_STATE::GETHIT:
		_state = S_MONSTER_STATE::GETHIT;
		SetState(&MonsterState::HitState::GetInstance());
		break;
    case S_MONSTER_STATE::DEATH:
        _state = S_MONSTER_STATE::DEATH;
        SetState(&MonsterState::DeathState::GetInstance());
        break;
    case S_MONSTER_STATE::SPAWN:
        _state = S_MONSTER_STATE::SPAWN;
        SetState(&MonsterState::SpawnState::GetInstance());
        break;
	case S_MONSTER_STATE::UNDERGROUND:
		_state = S_MONSTER_STATE::UNDERGROUND;
		SetState(&MonsterState::UndergroundState::GetInstance());
		break;
	default:
        break;
    }
}

void Monster::Update()
{
    if (currentState) currentState->Update(this);
    LocalTransform(); // 바운딩 박스 업데이트 해주기
    SetTarget();
}

void Monster::TakeDamage(int damage)
{
    if (_barrier > 0) {
        _barrier -= damage;
        if (_barrier < 0) _barrier = 0;
    }
    else {
        _hp -= damage;
        if (_hp < 0) _hp = 0;
    }
    if (_hp > 0) {
        SetState(S_MONSTER_STATE::GETHIT);
    }
    else {
		SetState(S_MONSTER_STATE::DEATH);
    }
}

bool Monster::IsPlayerInRange(PlayerCharacter* target) const
{
	if (target == nullptr) return false; // 타겟이 없으면 false
	float distance = (_pos - target->_pos).LengthSquared();
    if (distance < pow(8.f, 2)) return true;
    return false;
}

bool Monster::IsPlayerTooMuchClose() const
{
    return (_target && (_pos - _target->_pos).LengthSquared() < pow(2.f, 2));
}

void Monster::SetTarget()
{
	float minDistance = 5000.f; // 걍 큰 수
    PlayerCharacter* close_player = nullptr;
	for (auto& player : _Player) {
		if (player == nullptr) continue; // 플레이어가 없으면 패스
        Vec3 playerPos = player->_pos;
		float distance = (_pos - playerPos).LengthSquared();
		// cout << distance << endl;
		if (distance < minDistance) {
			minDistance = distance;
            close_player = player;
		}
	}
	if (close_player && IsPlayerInRange(close_player)) {
		_target = close_player;
        Vec3 direction = _target->_pos - _pos;
        direction.y = 0.f;
        direction.Normalize();
        _look_dir = Vec3::Lerp(_look_dir, direction, 0.1f); // 부드러운 회전
        _velocity = direction * _speed; // 타겟 방향으로 이동 속도 설정
	}
	else {
		_target = nullptr; 
	}
}

void Monster::AvoidCollision(const unordered_map<int, Monster>& monsters)
{
	const float minDistance = _boundingbox.Extents.x * 2; // 최소 거리
	const float repelStrength = 0.02f; // 반발력 약화 (0.1f -> 0.02f)

	for (const auto& pair : monsters) {
		Monster* other = const_cast<Monster*>(&pair.second); // const 제거 (주의)
		if (other == this) continue;

		Vec3 delta = other->_pos - _pos;
		float distanceSq = delta.LengthSquared();

		if (distanceSq > 0.0f && distanceSq < minDistance * minDistance) {
			float distance = sqrt(distanceSq);
			if (distance < 0.001f) distance = 0.001f;
			Vec3 repel = -delta / distance * (minDistance - distance) * repelStrength;
			_pos += repel;

			//Vec3 newDirection = _velocity - (delta / distance) * 0.05f;
			//if (newDirection.LengthSquared() > 0.001f) {
			//	_velocity = newDirection.Normalize() * _speed.Length();
			//}
		}
	}
}

void Monster::ReadAnimationInfo(const std::string& fileName)
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