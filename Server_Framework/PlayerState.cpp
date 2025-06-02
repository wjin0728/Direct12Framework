#include "PlayerState.h"
#include "PlayerCharacter.h"

// PlayerState::IdleState 구현
PlayerState::IdleState& PlayerState::IdleState::GetInstance() { static PlayerState::IdleState instance; return instance; }

void PlayerState::IdleState::Enter(PlayerCharacter* player) {
    cout << "Idle 들어왔다리!" << endl;
    player->SetVelocity(0, 0, 0); // 속도 0으로 설정
}

void PlayerState::IdleState::Update(PlayerCharacter* player) {
    // cout << "IDLE 업데이트 중!" << endl;
    if (player->HasMoveInput()) {
        player->SetState(&PlayerState::RunState::GetInstance());
    }
}

void PlayerState::IdleState::Exit(PlayerCharacter* player) {}

// PlayerState::RunState 구현
PlayerState::RunState& PlayerState::RunState::GetInstance() { static PlayerState::RunState instance; return instance; }

void PlayerState::RunState::Enter(PlayerCharacter* player) {
	cout << "RUN 들어왔다리!" << endl;
}

void PlayerState::RunState::Update(PlayerCharacter* player) {
    // cout << "RUN 업데이트 중!" << endl;
    if (!player->HasMoveInput()) {
        player->SetState(&PlayerState::IdleState::GetInstance());
        return;
    }

    player->_pos += (player->_velocity * TICK_INTERVAL);
}

void PlayerState::RunState::Exit(PlayerCharacter* player) {}

// PlayerState::BasicAttackState 구현
PlayerState::BasicAttackState& PlayerState::BasicAttackState::GetInstance() { static PlayerState::BasicAttackState instance; return instance; }
void PlayerState::BasicAttackState::Enter(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0); // 공격 중 이동 멈춤
	cout << "BasicAttack 들어왔다리!" << endl;

    switch (player->_class)
    {
    case S_PLAYER_CLASS::FIGHTER:
        break;
	case S_PLAYER_CLASS::ARCHER:
        break;
	case S_PLAYER_CLASS::MAGE:
        break;
    default:
        break;
    }
}

void PlayerState::BasicAttackState::Update(PlayerCharacter* player) {
	attackTimer += TICK_INTERVAL;   
	// cout << "BasicAttack 업데이트 중!" << endl;


}

void PlayerState::BasicAttackState::Exit(PlayerCharacter* player) {}


// PlayerState::RunAttackState 구현
PlayerState::RunAttackState& PlayerState::RunAttackState::GetInstance() { static PlayerState::RunAttackState instance; return instance; }

void PlayerState::RunAttackState::Enter(PlayerCharacter* player) {
    cout << "RunAttack 들어왔다리!" << endl;
}

void PlayerState::RunAttackState::Update(PlayerCharacter* player) {
    attackTimer += TICK_INTERVAL;
	// cout << "RunAttack 업데이트 중!" << endl;

    if (!player->HasMoveInput()) {
        player->SetState(&PlayerState::IdleState::GetInstance());
        return;
    }

    //player->_pos += (player->_velocity * TICK_INTERVAL);
}

void PlayerState::RunAttackState::Exit(PlayerCharacter* player) {}

// PlayerState::JumpState 구현
PlayerState::JumpState& PlayerState::JumpState::GetInstance() { static PlayerState::JumpState instance; return instance; }

void PlayerState::JumpState::Enter(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0);
    cout << "Jump 들어왔다리!" << endl;
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
	cout << "Skill 들어왔다리!" << endl;
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
	//cout << "Skill 업데이트 중!" << endl;
}

void PlayerState::SkillState::Exit(PlayerCharacter* player) {}

// PlayerState::HitState 구현
PlayerState::HitState& PlayerState::HitState::GetInstance() { static PlayerState::HitState instance; return instance; }

void PlayerState::HitState::Enter(PlayerCharacter* player) {
	cout << "Hit 들어왔다리!" << endl;

}

void PlayerState::HitState::Update(PlayerCharacter* player) {
	//cout << "Hit 업데이트 중!" << endl;
}

void PlayerState::HitState::Exit(PlayerCharacter* player) {

}