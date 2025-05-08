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
			previousState = currentState; // HitState�� ��ȯ �� ���� ���� ����
		}
		currentState->Exit(this);
	}
	currentState = newState;
	if (currentState) currentState->Enter(this);
}

void PlayerCharacter::Update() 
{
	if (currentState) currentState->Update(this);
	LocalTransform(); // �ٿ�� �ڽ� ������Ʈ
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
		// ��� ó�� (����: ���� ��ȯ ����)
	}
}
