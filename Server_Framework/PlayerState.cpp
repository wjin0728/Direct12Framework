#include "PlayerState.h"
#include "PlayerCharacter.h"

// PlayerState::IdleState 구현
PlayerState::IdleState& PlayerState::IdleState::GetInstance() { static PlayerState::IdleState instance; return instance; }

void PlayerState::IdleState::Enter(PlayerCharacter* player) {
    //cout << "Idle 들어왔다리!" << endl;
    player->SetVelocity(0, 0, 0); // 속도 0으로 설정
}

void PlayerState::IdleState::Update(PlayerCharacter* player) {
    cout << "IDLE 업데이트 중!" << endl;
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

// PlayerState::RunState 구현
PlayerState::RunState& PlayerState::RunState::GetInstance() { static PlayerState::RunState instance; return instance; }

void PlayerState::RunState::Enter(PlayerCharacter* player) {
    // 이동 애니메이션 시작 (가정)
	cout << "RUN 들어왔다리!" << endl;
}

void PlayerState::RunState::Update(PlayerCharacter* player) {
    cout << "RUN 업데이트 중!" << endl;
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
    attackTimer = 1.0f; // 공격 지속 시간 (1초)
    player->SetVelocity(0, 0, 0); // 공격 중 이동 멈춤
	cout << "BasicAttack 들어왔다리!" << endl;
    // 공격 애니메이션 시작 (가정)
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

// PlayerState::SkillState 구현
PlayerState::SkillState& PlayerState::SkillState::GetInstance() { static PlayerState::SkillState instance; return instance; }

void PlayerState::SkillState::Enter(PlayerCharacter* player) {
    skillTimer = 2.0f; // 스킬 지속 시간 (2초)
    player->SetVelocity(0, 0, 0); // 스킬 중 이동 멈춤
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
    skillTimer -= TICK_INTERVAL;
    if (skillTimer <= 0) {
        player->SetState(&PlayerState::IdleState::GetInstance());
    }
}

void PlayerState::SkillState::Exit(PlayerCharacter* player) {}

// PlayerState::HitState 구현
PlayerState::HitState& PlayerState::HitState::GetInstance() { static PlayerState::HitState instance; return instance; }

void PlayerState::HitState::Enter(PlayerCharacter* player) {
    hitTimer = 0.5f; // 피격 상태 지속 시간 (0.5초)
    player->SetVelocity(0, 0, 0); // 이동 멈춤
    // 피격 애니메이션 시작 (가정)
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
    // 피격 애니메이션 종료 (가정)
}