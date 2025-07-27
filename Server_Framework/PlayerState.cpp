#include "PlayerState.h"
#include "PlayerCharacter.h"

// PlayerState::IdleState 구현
PlayerState::IdleState& PlayerState::IdleState::GetInstance() { static PlayerState::IdleState instance; return instance; }

void PlayerState::IdleState::Enter(PlayerCharacter* player) {
    //cout << "Idle 들어왔다리!" << endl;
    player->SetVelocity(0, 0, 0); // 속도 0으로 설정
}

void PlayerState::IdleState::Update(PlayerCharacter* player) {
    // cout << "IDLE 업데이트 중!" << endl;
    if (player->HasMoveInput()) {
        player->SetState(&PlayerState::RunState::GetInstance());
    }
}

void PlayerState::IdleState::Exit(PlayerCharacter* player) {
}

// PlayerState::RunState 구현
PlayerState::RunState& PlayerState::RunState::GetInstance() { static PlayerState::RunState instance; return instance; }

void PlayerState::RunState::Enter(PlayerCharacter* player) {

}

void PlayerState::RunState::Update(PlayerCharacter* player) {
    // cout << "RUN 업데이트 중!" << endl;
    if (!player->HasMoveInput()) {
        player->SetState(&PlayerState::IdleState::GetInstance());
        return;
    }

    //player->_pos += (player->_velocity * TICK_INTERVAL);
}

void PlayerState::RunState::Exit(PlayerCharacter* player) {}

// PlayerState::BasicAttackState 구현
PlayerState::BasicAttackState& PlayerState::BasicAttackState::GetInstance() { static PlayerState::BasicAttackState instance; return instance; }
void PlayerState::BasicAttackState::Enter(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0); // 공격 중 이동 멈춤
	//cout << "BasicAttack 들어왔다리!" << endl;

    switch (player->_class)
    {
    case S_PLAYER_CLASS::FIGHTER:
        break;
	case S_PLAYER_CLASS::ARCHER:
        player->SetTarget();
        break;
	case S_PLAYER_CLASS::MAGE:
        player->SetTarget();
        break;
    default:
        break;
    }
}

void PlayerState::BasicAttackState::Update(PlayerCharacter* player) {
	attackTimer += TICK_INTERVAL;   
    player->SetVelocity(0, 0, 0); // 속도 0으로 설정
    // cout << "BasicAttack 업데이트 중!" << endl;

    if (player->_target) {
        Vec3 direction = player->_target->_pos - player->_pos;
        Vec3 look = { sin(player->_look_dir.y * degToRad), 0.0f, cos(player->_look_dir.y * degToRad) };
        direction.y = 0.f;
        direction.Normalize();

        direction = Vec3::Lerp(look, direction, 0.3f);

        Quaternion targetRot = Quaternion::LookRotation(direction);
        Vec3 angle = Vec3::GetAngleToQuaternion(targetRot) * radToDeg;
        player->_rotation = targetRot;
        player->SetLookDir(angle);
    }
}

void PlayerState::BasicAttackState::Exit(PlayerCharacter* player) {}


// PlayerState::RunAttackState 구현
PlayerState::RunAttackState& PlayerState::RunAttackState::GetInstance() { static PlayerState::RunAttackState instance; return instance; }

void PlayerState::RunAttackState::Enter(PlayerCharacter* player) {
    //cout << "RunAttack 들어왔다리!" << endl;
    switch (player->_class)
    {
    case S_PLAYER_CLASS::FIGHTER:
        break;
    case S_PLAYER_CLASS::ARCHER:
        player->SetTarget();
        break;
    case S_PLAYER_CLASS::MAGE:
        player->SetTarget();
        break;
    default:
        break;
    }

}

void PlayerState::RunAttackState::Update(PlayerCharacter* player) {
    attackTimer += TICK_INTERVAL;
	// cout << "RunAttack 업데이트 중!" << endl;

    if (!player->HasMoveInput()) {
        player->SetState(&PlayerState::IdleState::GetInstance());
        return;
    }

    if (player->_target) {
        Vec3 direction = player->_target->_pos - player->_pos;
        Vec3 look = { sin(player->_look_dir.y * degToRad), 0.0f, cos(player->_look_dir.y * degToRad) };
        direction.y = 0.f;
        direction.Normalize();

        direction = Vec3::Lerp(look, direction, 0.3f);

        Quaternion targetRot = Quaternion::LookRotation(direction);
        Vec3 angle = Vec3::GetAngleToQuaternion(targetRot) * radToDeg;
        player->_rotation = targetRot;
        player->SetLookDir(angle);
    }

    //player->_pos += (player->_velocity * TICK_INTERVAL);
}

void PlayerState::RunAttackState::Exit(PlayerCharacter* player) {}

// PlayerState::JumpState 구현
PlayerState::JumpState& PlayerState::JumpState::GetInstance() { static PlayerState::JumpState instance; return instance; }

void PlayerState::JumpState::Enter(PlayerCharacter* player) {
    //player->SetVelocity(0, 0, 0);
    //cout << "Jump 들어왔다리!" << endl;
}

void PlayerState::JumpState::Update(PlayerCharacter* player) {
	// cout << "Jump 업데이트 중!" << endl;
    //player->_pos += (player->_velocity * TICK_INTERVAL);
}

void PlayerState::JumpState::Exit(PlayerCharacter* player) {}


// PlayerState::SkillState 구현
PlayerState::SkillState& PlayerState::SkillState::GetInstance() { static PlayerState::SkillState instance; return instance; }

void PlayerState::SkillState::Enter(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0); // 스킬 중 이동 멈춤
	//cout << "Skill 들어왔다리!" << endl;
    // 스킬별 동작 호출
    //switch (player->GetSkill()) {
    //case S_ITEM_TYPE::FIRE_ENCHANT: player->OnSkillFireEnchant(); break;
    //case S_ITEM_TYPE::FIRE_EXPLOSION: player->OnSkillFireExplosion(); break;
    //case S_ITEM_TYPE::WATER_HEAL: player->OnSkillWaterHeal(); break;
    //case S_ITEM_TYPE::WATER_SHIELD: player->OnSkillWaterShield(); break;
    //case S_ITEM_TYPE::GRASS_WEAKEN: player->OnSkillGrassWeaken(); break;
    //case S_ITEM_TYPE::GRASS_VINE: player->OnSkillGrassVine(); break;
    //default: break;
    //}
}

void PlayerState::SkillState::Update(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0); // 스킬 중 이동 멈춤
    //cout << "Skill 업데이트 중!" << endl;
}

void PlayerState::SkillState::Exit(PlayerCharacter* player) {}

// PlayerState::HitState 구현
PlayerState::HitState& PlayerState::HitState::GetInstance() { static PlayerState::HitState instance; return instance; }

void PlayerState::HitState::Enter(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0);
}

void PlayerState::HitState::Update(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0);
}

void PlayerState::HitState::Exit(PlayerCharacter* player) {

}

// PlayerState::UltimateState 구현
PlayerState::UltimateState& PlayerState::UltimateState::GetInstance() { static PlayerState::UltimateState instance; return instance; }

void PlayerState::UltimateState::Enter(PlayerCharacter* player) {
    ultimateTimer = 0.f;
    player->_data = 0.f;
    player->SetVelocity(0, 0, 0); // 스킬 중 이동 멈춤
    player->SetTarget();

    if (player->_target) {
        Vec3 direction = player->_target->_pos - player->_pos;
        direction.y = 0.f;
        direction.Normalize();

        Quaternion targetRot = Quaternion::LookRotation(direction);
        Vec3 angle = Vec3::GetAngleToQuaternion(targetRot) * radToDeg;

        player->_rotation = targetRot;
        player->SetLookDir(angle);

        if (player->_class == S_PLAYER_CLASS::FIGHTER) {
            startPos = player->_pos;
            targetPos = player->_target->_pos;
        }
    }
}

void PlayerState::UltimateState::Update(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0); // 속도 0으로 설정

    if (player->_class == S_PLAYER_CLASS::FIGHTER) {
        ultimateTimer += TICK_INTERVAL;

        float start = 1.3;
        float peak = 1.5;
        float mid = 1.9;
        float end = 2.1;
        float maxHeight = 2.f;
		float moveDuration = end - start;

        if (start <= ultimateTimer && ultimateTimer < peak)
            player->_data = -50.f * (ultimateTimer - peak) * (ultimateTimer - peak) + maxHeight;
        else if (peak <= ultimateTimer && ultimateTimer < mid)
            player->_data = maxHeight;
        else if (mid <= ultimateTimer && ultimateTimer <= end)
            player->_data = -50.f * (ultimateTimer - mid) * (ultimateTimer - mid) + maxHeight;
    
        // 이동 진행
        if (start <= ultimateTimer && ultimateTimer < end) {
            float t = (ultimateTimer - start) / moveDuration;
            t = std::clamp(t, 0.f, 1.f);

            Vec3 flatMove = Vec3::Lerp(startPos, targetPos, t);
            player->_pos.x = flatMove.x;
            player->_pos.z = flatMove.z;
        }
    }
}

void PlayerState::UltimateState::Exit(PlayerCharacter* player) {
    player->_data = 0.f;
}

// PlayerState::GatheringState 구현
PlayerState::GatheringState& PlayerState::GatheringState::GetInstance() { static PlayerState::GatheringState instance; return instance; }

void PlayerState::GatheringState::Enter(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0);
}

void PlayerState::GatheringState::Update(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0); // 속도 0으로 설정
}

void PlayerState::GatheringState::Exit(PlayerCharacter* player) {
}

// PlayerState::DeathState 구현
PlayerState::DeathState& PlayerState::DeathState::GetInstance() { static PlayerState::DeathState instance; return instance; }

void PlayerState::DeathState::Enter(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0);
}

void PlayerState::DeathState::Update(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0); // 속도 0으로 설정
}

void PlayerState::DeathState::Exit(PlayerCharacter* player) {
    player->_pos = player->_spawn_pos;
    player->_rotation = player->_spawn_rotation;
}
