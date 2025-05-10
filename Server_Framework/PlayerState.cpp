#include "PlayerState.h"
#include "PlayerCharacter.h"

// PlayerState::IdleState ����
PlayerState::IdleState& PlayerState::IdleState::GetInstance() { static PlayerState::IdleState instance; return instance; }

void PlayerState::IdleState::Enter(PlayerCharacter* player) {
    //cout << "Idle ���Դٸ�!" << endl;
    player->SetVelocity(0, 0, 0); // �ӵ� 0���� ����
}

void PlayerState::IdleState::Update(PlayerCharacter* player) {
    cout << "IDLE ������Ʈ ��!" << endl;
    if (player->HasMoveInput()) {
        player->SetState(&PlayerState::RunState::GetInstance());
    }
    //else if (player->HasAttackInput()) {
    //    player->SetState(&PlayerState::BasicAttackState::GetInstance());
    //}
    //else if (player->HasSkillInput()) {
    //    player->SetState(&PlayerState::SkillState::GetInstance());
    //}
}

void PlayerState::IdleState::Exit(PlayerCharacter* player) {}

// PlayerState::RunState ����
PlayerState::RunState& PlayerState::RunState::GetInstance() { static PlayerState::RunState instance; return instance; }

void PlayerState::RunState::Enter(PlayerCharacter* player) {
    // �̵� �ִϸ��̼� ���� (����)
	cout << "RUN ���Դٸ�!" << endl;
}

void PlayerState::RunState::Update(PlayerCharacter* player) {
    cout << "RUN ������Ʈ ��!" << endl;
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
    attackTimer = 1.0f; // ���� ���� �ð� (1��)
    player->SetVelocity(0, 0, 0); // ���� �� �̵� ����
	cout << "BasicAttack ���Դٸ�!" << endl;
    // ���� �ִϸ��̼� ���� (����)
}

void PlayerState::BasicAttackState::Update(PlayerCharacter* player) {
    attackTimer -= TICK_INTERVAL;
    if (attackTimer <= 0) {
        //if (player->HasMoveInput()) {
        //    player->SetState(&PlayerState::RunState::GetInstance());
        //}
        //else {
        //    player->SetState(&PlayerState::IdleState::GetInstance());
        //}
    }
}

void PlayerState::BasicAttackState::Exit(PlayerCharacter* player) {}

// PlayerState::SkillState ����
PlayerState::SkillState& PlayerState::SkillState::GetInstance() { static PlayerState::SkillState instance; return instance; }

void PlayerState::SkillState::Enter(PlayerCharacter* player) {
    skillTimer = 2.0f; // ��ų ���� �ð� (2��)
    player->SetVelocity(0, 0, 0); // ��ų �� �̵� ����
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
    skillTimer -= TICK_INTERVAL;
    if (skillTimer <= 0) {
        player->SetState(&PlayerState::IdleState::GetInstance());
    }
}

void PlayerState::SkillState::Exit(PlayerCharacter* player) {}

// PlayerState::HitState ����
PlayerState::HitState& PlayerState::HitState::GetInstance() { static PlayerState::HitState instance; return instance; }

void PlayerState::HitState::Enter(PlayerCharacter* player) {
    hitTimer = 0.5f; // �ǰ� ���� ���� �ð� (0.5��)
    player->SetVelocity(0, 0, 0); // �̵� ����
    // �ǰ� �ִϸ��̼� ���� (����)
}

void PlayerState::HitState::Update(PlayerCharacter* player) {
    hitTimer -= TICK_INTERVAL;
    if (hitTimer <= 0) {
        //if (player->HasMoveInput()) {
        //    player->SetState(&PlayerState::RunState::GetInstance());
        //}
        //else {
        //    player->SetState(&PlayerState::IdleState::GetInstance());
        //}
    }
}

void PlayerState::HitState::Exit(PlayerCharacter* player) {
    // �ǰ� �ִϸ��̼� ���� (����)
}