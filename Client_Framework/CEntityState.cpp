#include "stdafx.h"
#include "CEntityState.h"
#include"HealthSystem.h"

void CEntityState::GetHit(float damage)
{
	if (mIsDead) return;
	mHealth -= damage;
	if (mHealthSystem.lock()) {
		mHealthSystem.lock()->GetDamage(damage);
	}
	if (mHealth <= 0) {
		mHealth = 0;
		mIsDead = true;
	}
}

void CEntityState::Heal(float amount)
{
	if (mIsDead) return;
	mHealth += amount;
	if (mHealthSystem.lock()) {
		mHealthSystem.lock()->Heal(amount);
	}
	if (mHealth > mMaxHealth) {
		mHealth = mMaxHealth;
	}
}