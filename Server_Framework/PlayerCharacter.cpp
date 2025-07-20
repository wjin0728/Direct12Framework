#include "PlayerCharacter.h"

void PlayerCharacter::OnSkillFireEnchant()
{
}

void PlayerCharacter::OnSkillFireExplosion()
{
}

void PlayerCharacter::OnSkillWaterHeal()
{
	auto maxhp = PlayerMaxHp();
	if (_hp < maxhp) {
		_hp += WATER_HEAL_AMT;
		if (_hp > maxhp) _hp = maxhp;
	}
}

void PlayerCharacter::OnSkillWaterShield()
{
	_barrier = 2;
}

void PlayerCharacter::OnSkillGrassWeaken()
{
}

void PlayerCharacter::OnSkillGrassVine()
{
}

void PlayerCharacter::SetState(PlayerStateMachine* newState) 
{
	if (currentState) {
		if (newState != &PlayerState::HitState::GetInstance()) {
			previousState = currentState; // HitState로 전환 시 현재 상태 저장
		}
		currentState->Exit(this);
	}
	currentState = newState;
	if (currentState) currentState->Enter(this);
}

void PlayerCharacter::SetState(uint8_t newState)
{
	//if (newState == (uint8_t)_state) return;
    _time = 0.0f;

    switch ((S_PLAYER_STATE)newState) {
    case S_PLAYER_STATE::IDLE:
        _state = S_PLAYER_STATE::IDLE;
        SetState(&PlayerState::IdleState::GetInstance());
        break;
    case S_PLAYER_STATE::RUN:
        _state = S_PLAYER_STATE::RUN;
        SetState(&PlayerState::RunState::GetInstance());
        break;
    case S_PLAYER_STATE::ATTACK:
        _state = S_PLAYER_STATE::ATTACK;
        SetState(&PlayerState::BasicAttackState::GetInstance());
        break;
    case S_PLAYER_STATE::RUNATTACK:
        _state = S_PLAYER_STATE::RUNATTACK;
        SetState(&PlayerState::RunAttackState::GetInstance());
        break;
    case S_PLAYER_STATE::JUMP:
        _state = S_PLAYER_STATE::JUMP;
        SetState(&PlayerState::JumpState::GetInstance());
        break;
    case S_PLAYER_STATE::SKILL:
        _state = S_PLAYER_STATE::SKILL;
        SetState(&PlayerState::SkillState::GetInstance());
        break;
    case S_PLAYER_STATE::ULTIMATE:
        _state = S_PLAYER_STATE::ULTIMATE;
        SetState(&PlayerState::UltimateState::GetInstance());
        break;
    case S_PLAYER_STATE::GATHERING:
        _state = S_PLAYER_STATE::GATHERING;
        SetState(&PlayerState::GatheringState::GetInstance());
        break;
    default:
        break;
    }
}
void PlayerCharacter::Update() 
{
	if (currentState) currentState->Update(this);
    _time += TICK_INTERVAL;
	LocalTransform();
}

void PlayerCharacter::TakeDamage(int damage) 
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
		SetState(&PlayerState::HitState::GetInstance());
	}
	else {
		// 사망 처리
	}
}

void PlayerCharacter::SetTarget()
{
    float minDistance = 5000.f; // 걍 큰 수
    Monster* close_monster = nullptr;

    for (auto& monster : _Monster) {
        if (monster == nullptr || monster->_remove) continue;
        Vec3 monsterPos = monster->_pos;
        float distance = (_pos - monsterPos).LengthSquared();

        if (distance < minDistance) {
            minDistance = distance;
            close_monster = monster;
        }
    }

    if (close_monster && IsMonsterInRange(close_monster)) {
        _target = close_monster;
    }
    else {
        _target = nullptr;
    }
}

bool PlayerCharacter::IsMonsterInRange(Monster* target) const
{
    if (target == nullptr) return false; // 타겟이 없으면 false
    float distance = (_pos - target->_pos).LengthSquared();
    if (distance < pow(8.f, 2)) return true;
    return false;
}

//const std::array<float, 2>& center, float radius, const std::array<float, 2>& forward, float sectorAngle,
//const std::vector<std::array<float, 2>>& rectCorners

bool PlayerCharacter::OnFighterBasicAttack(BoundingOrientedBox& monster_box) {
    BoundingOrientedBox atbox;
    XMVECTOR center = XMLoadFloat3(&_boundingbox.Center);
    float yawRad = XMConvertToRadians(_look_dir.y); // _look_dir.y를 라디안으로
    XMVECTOR forward = XMVectorSet(sinf(yawRad), 0.0f, cosf(yawRad), 0.0f); // 방향 벡터
    forward = XMVector3Normalize(forward); // 정규화
    float offset = 0.2f; // 전사 앞 거리

    XMVECTOR attackBoxCenter = XMVectorAdd(center, XMVectorScale(forward, offset));
    XMStoreFloat3(&atbox.Center, attackBoxCenter);

    atbox.Extents = Vec3(0.6801331f, 1.110679f, 0.5878519f) / 4.f;

    atbox.Orientation = _rotation; // _rotation 사용

	cout << "Attack Box Center: " << atbox.Center.x << ", " << atbox.Center.y << ", " << atbox.Center.z << endl;
	cout << "Monster Box Center: " << monster_box.Center.x << ", " << monster_box.Center.y << ", " << monster_box.Center.z << endl;
	cout << "_boundingbox Center: " << _boundingbox.Center.x << ", " << _boundingbox.Center.y << ", " << _boundingbox.Center.z << endl;
	if (atbox.Intersects(monster_box)) return true; 
    return false;
}