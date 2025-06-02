#include "PlayerState.h"
#include "PlayerCharacter.h"

// PlayerState::IdleState ����
PlayerState::IdleState& PlayerState::IdleState::GetInstance() { static PlayerState::IdleState instance; return instance; }

void PlayerState::IdleState::Enter(PlayerCharacter* player) {
    cout << "Idle ���Դٸ�!" << endl;
    player->SetVelocity(0, 0, 0); // �ӵ� 0���� ����
}

void PlayerState::IdleState::Update(PlayerCharacter* player) {
    // cout << "IDLE ������Ʈ ��!" << endl;
    if (player->HasMoveInput()) {
        player->SetState(&PlayerState::RunState::GetInstance());
    }
}

void PlayerState::IdleState::Exit(PlayerCharacter* player) {}

// PlayerState::RunState ����
PlayerState::RunState& PlayerState::RunState::GetInstance() { static PlayerState::RunState instance; return instance; }

void PlayerState::RunState::Enter(PlayerCharacter* player) {
	cout << "RUN ���Դٸ�!" << endl;
}

void PlayerState::RunState::Update(PlayerCharacter* player) {
    // cout << "RUN ������Ʈ ��!" << endl;
    if (!player->HasMoveInput()) {
        player->SetState(&PlayerState::IdleState::GetInstance());
        return;
    }

    player->_pos += (player->_velocity * TICK_INTERVAL);
}

void PlayerState::RunState::Exit(PlayerCharacter* player) {}

// PlayerState::BasicAttackState ����
PlayerState::BasicAttackState& PlayerState::BasicAttackState::GetInstance() { static PlayerState::BasicAttackState instance; return instance; }
void PlayerState::BasicAttackState::Enter(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0); // ���� �� �̵� ����
	cout << "BasicAttack ���Դٸ�!" << endl;

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
	// cout << "BasicAttack ������Ʈ ��!" << endl;


}

void PlayerState::BasicAttackState::Exit(PlayerCharacter* player) {}


// PlayerState::RunAttackState ����
PlayerState::RunAttackState& PlayerState::RunAttackState::GetInstance() { static PlayerState::RunAttackState instance; return instance; }

void PlayerState::RunAttackState::Enter(PlayerCharacter* player) {
    cout << "RunAttack ���Դٸ�!" << endl;
}

void PlayerState::RunAttackState::Update(PlayerCharacter* player) {
    attackTimer += TICK_INTERVAL;
	// cout << "RunAttack ������Ʈ ��!" << endl;

    if (!player->HasMoveInput()) {
        player->SetState(&PlayerState::IdleState::GetInstance());
        return;
    }

    //player->_pos += (player->_velocity * TICK_INTERVAL);
}

void PlayerState::RunAttackState::Exit(PlayerCharacter* player) {}

// PlayerState::JumpState ����
PlayerState::JumpState& PlayerState::JumpState::GetInstance() { static PlayerState::JumpState instance; return instance; }

void PlayerState::JumpState::Enter(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0);
    cout << "Jump ���Դٸ�!" << endl;
}

void PlayerState::JumpState::Update(PlayerCharacter* player) {
	// cout << "Jump ������Ʈ ��!" << endl;
    //player->_pos += (player->_velocity * TICK_INTERVAL);
}

void PlayerState::JumpState::Exit(PlayerCharacter* player) {}


// PlayerState::SkillState ����
PlayerState::SkillState& PlayerState::SkillState::GetInstance() { static PlayerState::SkillState instance; return instance; }

void PlayerState::SkillState::Enter(PlayerCharacter* player) {
    player->SetVelocity(0, 0, 0); // ��ų �� �̵� ����
	cout << "Skill ���Դٸ�!" << endl;
    // ��ų�� ���� ȣ��
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
	//cout << "Skill ������Ʈ ��!" << endl;
}

void PlayerState::SkillState::Exit(PlayerCharacter* player) {}

// PlayerState::HitState ����
PlayerState::HitState& PlayerState::HitState::GetInstance() { static PlayerState::HitState instance; return instance; }

void PlayerState::HitState::Enter(PlayerCharacter* player) {
	cout << "Hit ���Դٸ�!" << endl;

}

void PlayerState::HitState::Update(PlayerCharacter* player) {
	//cout << "Hit ������Ʈ ��!" << endl;
}

void PlayerState::HitState::Exit(PlayerCharacter* player) {

}