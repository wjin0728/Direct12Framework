#include "Monster.h"
#include "PlayerCharacter.h"

void Monster::SetState(MonsterStateMachine* newState)
{
    if (currentState) {
        if (newState == &MonsterState::HitState::GetInstance() || newState == &MonsterState::BossHitState::GetInstance()) {
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
	_animation_time = 0.f;

	if (newState == S_MONSTER_STATE::ATTACK) {
		for (auto& key : _animations[(int)S_MONSTER_STATE::ATTACK].mEventKeys) { key.mEnable = true; }
		for (auto& key : _animations[(int)S_MONSTER_STATE::ATTACK2].mEventKeys) { key.mEnable = true; }
		for (auto& key : _animations[(int)S_MONSTER_STATE::SKILL].mEventKeys) { key.mEnable = true; }
	}

	if (_class == S_ENEMY_TYPE::BOSS) {
		SetBossState(newState);
	}
	else {
		SetMonsterState(newState);
	}
}
void Monster::SetMonsterState(S_MONSTER_STATE newState)
{
	switch (newState) {
	case S_MONSTER_STATE::IDLE:
		SetState(&MonsterState::IdleState::GetInstance());
		_state = S_MONSTER_STATE::IDLE;
		break;
	case S_MONSTER_STATE::RUN:
		SetState(&MonsterState::RunState::GetInstance());
		_state = S_MONSTER_STATE::RUN;
		break;
	case S_MONSTER_STATE::ATTACK:
		SetState(&MonsterState::AttackState::GetInstance());
		_state = S_MONSTER_STATE::ATTACK;
		break;
	case S_MONSTER_STATE::GETHIT:
		SetState(&MonsterState::HitState::GetInstance());
		_state = S_MONSTER_STATE::GETHIT;
		break;
	case S_MONSTER_STATE::DEATH:
		SetState(&MonsterState::DeathState::GetInstance());
		_state = S_MONSTER_STATE::DEATH;
		break;
	case S_MONSTER_STATE::SPAWN:
		SetState(&MonsterState::SpawnState::GetInstance());
		_state = S_MONSTER_STATE::SPAWN;
		break;
	case S_MONSTER_STATE::UNDERGROUND:
		SetState(&MonsterState::UndergroundState::GetInstance());
		_state = S_MONSTER_STATE::UNDERGROUND;
		break;
	default:
		break;
	}
}
void Monster::SetBossState(S_MONSTER_STATE newState)
{
	switch (newState) {
	case S_MONSTER_STATE::IDLE:
		SetState(&MonsterState::BossIdleState::GetInstance());
		_state = S_MONSTER_STATE::IDLE;
		break;
	case S_MONSTER_STATE::ATTACK:
		SetState(&MonsterState::BossAttackState::GetInstance());
		_state = S_MONSTER_STATE::ATTACK;
		break;
	case S_MONSTER_STATE::SKILL:
		SetState(&MonsterState::BossAttackState::GetInstance());
		_state = S_MONSTER_STATE::SKILL;
		break;
	case S_MONSTER_STATE::GETHIT:
		SetState(&MonsterState::BossHitState::GetInstance());
		_state = S_MONSTER_STATE::GETHIT;
		break;
	case S_MONSTER_STATE::DEATH:
		SetState(&MonsterState::BossDeathState::GetInstance());
		_state = S_MONSTER_STATE::DEATH;
		break;
	case S_MONSTER_STATE::SPAWN:
		SetState(&MonsterState::BossSpawnState::GetInstance());
		_state = S_MONSTER_STATE::SPAWN;
		break;
	case S_MONSTER_STATE::UNDERGROUND:
		SetState(&MonsterState::BossUndergroundState::GetInstance());
		_state = S_MONSTER_STATE::UNDERGROUND;
		break;
	default:
		break;
	}
}

void Monster::Update()
{
	_animation_time += TICK_INTERVAL;
	if (_on_CantMove&&cant_move_time>0.f) {
		cant_move_time -= TICK_INTERVAL;
		if (cant_move_time <= 0.f) {
			_on_CantMove = false;
			cant_move_time = 0.f;
		}
	}
    if (currentState) currentState->Update(this);
    LocalTransform(); // 바운딩 박스 업데이트 해주기
    if (_class != S_ENEMY_TYPE::BOSS) UpdateTarget();
}

void Monster::TakeDamage(int damage, bool do_hit_raction)
{
    if (_barrier > 0) {
        _barrier -= damage;
        if (_barrier < 0) _barrier = 0;
    }
    else {
        _hp -= damage;
        if (_hp < 0) _hp = 0;
		std::cout << "Monster HP: " << _hp << std::endl;
		if (_hp > 0 && do_hit_raction) {
			SetState(S_MONSTER_STATE::GETHIT);
		}
		else if (_hp <= 0) {
			SetState(S_MONSTER_STATE::DEATH);
		}
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

void Monster::UpdateTarget()
{
	if (_class == S_ENEMY_TYPE::BOSS) {
		if (_target) {
			Vec3 direction = _target->_pos - _pos;
			direction.y = 0.f;
			direction.Normalize();
			_look_dir = Vec3::Lerp(_look_dir, direction, 0.1f); // 부드러운 회전
		}
		return;
	}

	float minDistance = 5000.f; // 걍 큰 수
    PlayerCharacter* close_player = nullptr;
	for (auto& player : _Player) {
		if (player == nullptr) continue; // 플레이어가 없으면 패스
		if (player->_state == S_PLAYER_STATE::DEATH) continue; // 플레이어가 없으면 패스
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

void Monster::SetRandomTarget()
{
	vector<int> available_index;

	for (int i = 0; auto& player : _Player) {
		if (player && player->_state != S_PLAYER_STATE::DEATH && player->_id != -1) {
			available_index.push_back(i);
		}
		++i;
	}

	if (available_index.size()) {
		int randomIndex{};
		randomIndex = rand() % available_index.size();
		_target = _Player[available_index[randomIndex]];
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
		}
		else if (token == "<AnimationSet>:") {
			mEventHandler.push_back(CAnimationEventHandler{}); // 이벤트 핸들러 초기화

			int setNum{}, framesPerSecondNum{}, keyFrameNum{};
			float length{};
			std::string setName;

			ReadDateFromFile(ifs, setNum);
			ReadDateFromFile(ifs, setName);
			ReadDateFromFile(ifs, length);
			ReadDateFromFile(ifs, framesPerSecondNum);
			ReadDateFromFile(ifs, keyFrameNum);

			AnimationInfo animSet{};
			animSet.mAnimationName = setName;
			animSet.mLength = length;
			animSet.mFrameLength = keyFrameNum;

			ReadDateFromFile(ifs, token);
			if (token == "<Loop>:") {
				int animationType{};
				ReadDateFromFile(ifs, animationType);
				animSet.mType = (ANIMATION_TYPE)animationType;
			}

			ReadDateFromFile(ifs, token);
			if (token == "<Events>:") {
				int eventCount{};
				ReadDateFromFile(ifs, eventCount);

				for (int i = 0; i < eventCount; ++i) {
					EventKey eventKey{};
					float eventTime{}, floatParam{};
					std::string eventStr;

					ReadDateFromFile(ifs, eventTime);
					ReadDateFromFile(ifs, floatParam);
					ReadDateFromFile(ifs, eventStr);

					eventKey.mTime = eventTime;
					eventKey.mData = floatParam;
					eventKey.mName = eventStr;

					animSet.mEventKeys.push_back(eventKey);
				}
			}

			_animations.push_back(animSet);
		}
		else if (token == "</AnimationSets>")
		{
			break;
		}
	}
}



void Monster::HandleCallback(CAnimationEventHandler& registry)
{
	for (auto& key : _animations[(int)_state].mEventKeys) {
		if (key.mTime < _animation_time && key.mEnable) {
			auto event = registry.GetEvent(key.mName);
			if (event) {
				event(this);
				key.mEnable = false;
			}
		}
	}
}
