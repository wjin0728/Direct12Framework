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

    switch (newState) {
    case (uint8_t)S_PLAYER_STATE::IDLE:
        _state = S_PLAYER_STATE::IDLE;
        SetState(&PlayerState::IdleState::GetInstance());
        break;
    case (uint8_t)S_PLAYER_STATE::RUN:
        _state = S_PLAYER_STATE::RUN;
        SetState(&PlayerState::RunState::GetInstance());
        break;
    case (uint8_t)S_PLAYER_STATE::ATTACK:
        _state = S_PLAYER_STATE::ATTACK;
        SetState(&PlayerState::BasicAttackState::GetInstance());
        break;
    case (uint8_t)S_PLAYER_STATE::RUNATTACK:
        _state = S_PLAYER_STATE::RUNATTACK;
        SetState(&PlayerState::RunAttackState::GetInstance());
        break;
    case (uint8_t)S_PLAYER_STATE::JUMP:
        _state = S_PLAYER_STATE::JUMP;
        SetState(&PlayerState::JumpState::GetInstance());
        break;
    case (uint8_t)S_PLAYER_STATE::SKILL:
        _state = S_PLAYER_STATE::SKILL;
        SetState(&PlayerState::SkillState::GetInstance());
        break;
    default:
        break;
    }
}
void PlayerCharacter::Update() 
{
	if (currentState) currentState->Update(this);
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

//const std::array<float, 2>& center, float radius, const std::array<float, 2>& forward, float sectorAngle,
//const std::vector<std::array<float, 2>>& rectCorners

bool PlayerCharacter::OnFighterBasicAttack(BoundingOrientedBox& monster_box) {
    const float PI = 3.1415926535f;
    float forwardAngle = std::atan2(_look_dir.x, _look_dir.z);
    float halfRadian = (80 * PI / 180.0f) / 2.0f;
    float alpha = forwardAngle - halfRadian;
    float beta = forwardAngle + halfRadian;
    float radius = 0.7f;

    Vec3 Corner[8];
	monster_box.GetCorners(Corner);
    int intcorner[4]{ 0, 1, 5, 4 };

    // 반지름 선분 끝점
    std::array<float, 2> Palpha = { _pos.x + radius * std::cos(alpha), _pos.z + radius * std::sin(alpha) };
    std::array<float, 2> Pbeta = { _pos.x + radius * std::cos(beta), _pos.z + radius * std::sin(beta) };

    // 사각형 변 순회
    for (size_t i = 0; i < 4; ++i) {
        size_t j = (i + 1) % 4;
        const auto& A = Corner[intcorner[i]];
        const auto& B = Corner[intcorner[j]];

        // 원호 교차 확인
        std::array<float, 2> A_rel = { A.x - _pos.x, A.z - _pos.z };
        std::array<float, 2> B_rel = { B.x - _pos.x, B.z - _pos.z };
        std::array<float, 2> d = { B_rel[0] - A_rel[0], B_rel[1] - A_rel[1] };
        float dr = d[0] * d[0] + d[1] * d[1];
        if (dr >= 1e-6f) {
            float D_cross = A_rel[0] * B_rel[1] - B_rel[0] * A_rel[1];
            float discriminant = radius * radius * dr - D_cross * D_cross;
            if (discriminant >= -1e-6f) {
                float sqrt_disc = std::sqrt(max(discriminant, 0.0f));
                float inv_dr = 1.0f / dr;
                float sign_dz = (d[1] < 0) ? -1.0f : 1.0f;

                // 첫 번째 교차점
                float x1 = (D_cross * d[1] + sign_dz * d[0] * sqrt_disc) * inv_dr;
                float z1 = (-D_cross * d[0] + std::fabs(d[1]) * sqrt_disc) * inv_dr;
                std::array<float, 2> P1_rel = { x1, z1 };
                float t1;
                if (std::fabs(d[0]) > std::fabs(d[1])) {
                    t1 = (P1_rel[0] - A_rel[0]) / d[0];
                }
                else {
                    t1 = (P1_rel[1] - A_rel[1]) / d[1];
                }
                if (t1 >= 0 && t1 <= 1) {
                    float angle1 = std::atan2(P1_rel[1], P1_rel[0]);
                    float angle_diff1 = std::fmod(angle1 - forwardAngle + 3 * PI, 2 * PI) - PI;
                    if (std::fabs(angle_diff1) <= halfRadian) {
                        cout << "Hit!!!!!!!!!" << endl;
                        return true;
                    }
                }

                // 두 번째 교차점
                float x2 = (D_cross * d[1] - sign_dz * d[0] * sqrt_disc) * inv_dr;
                float z2 = (-D_cross * d[0] - std::fabs(d[1]) * sqrt_disc) * inv_dr;
                std::array<float, 2> P2_rel = { x2, z2 };
                float t2;
                if (std::fabs(d[0]) > std::fabs(d[1])) {
                    t2 = (P2_rel[0] - A_rel[0]) / d[0];
                }
                else {
                    t2 = (P2_rel[1] - A_rel[1]) / d[1];
                }
                if (t2 >= 0 && t2 <= 1) {
                    float angle2 = std::atan2(P2_rel[1], P2_rel[0]);
                    float angle_diff2 = std::fmod(angle2 - forwardAngle + 3 * PI, 2 * PI) - PI;
                    if (std::fabs(angle_diff2) <= halfRadian) {
                        cout << "Hit!!!!!!!!!" << endl;
                        return true;
                    }
                }
            }
        }

        // 반지름 선분 교차 확인 (center-Palpha)
        {
            std::array<float, 2> C{ _pos.x, _pos.z };
            std::array<float, 2> D = Palpha;
            float d1 = (B.x - A.x) * (C[1] - A.z) - (B.z - A.z) * (C[0] - A.x) > 0;
            float d2 = (B.x - A.x) * (D[1] - A.z) - (B.z - A.z) * (D[0] - A.x) > 0;
            float d3 = (D[0] - C[0]) * (A.z - C[1]) - (D[1] - C[1]) * (A.x - C[0]) > 0;
            float d4 = (D[0] - C[0]) * (B.z - C[1]) - (D[1] - C[1]) * (B.x - C[0]) > 0;
            if ((d1 != d2) && (d3 != d4)) {
                cout << "Hit!!!!!!!!!" << endl;
                return true;
            }
        }

        // 반지름 선분 교차 확인 (center-Pbeta)
        {
            std::array<float, 2> C{ _pos.x, _pos.z };
            std::array<float, 2> D = Pbeta;
            float d1 = (B.x - A.x) * (C[1] - A.z) - (B.z - A.z) * (C[0] - A.x) > 0;
            float d2 = (B.x - A.x) * (D[1] - A.z) - (B.z - A.z) * (D[0] - A.x) > 0;
            float d3 = (D[0] - C[0]) * (A.z - C[1]) - (D[1] - C[1]) * (A.x - C[0]) > 0;
            float d4 = (D[0] - C[0]) * (B.z - C[1]) - (D[1] - C[1]) * (B.x - C[0]) > 0;
            if ((d1 != d2) && (d3 != d4)) {
                cout << "Hit!!!!!!!!!" << endl;
                return true;
            }
        }
    }

    return false;
}