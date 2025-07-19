#include "stdafx.h"
#include "EnemyState.h"
#include "Animation.h"
#include"Transform.h"
#include"ResourceManager.h"
#include"HealthSystem.h"

const float CEnemyState::MAX_HEALTH = 100.f;

void CEnemyState::Awake()
{

}

void CEnemyState::Start()
{
	auto healthBar = CGameObject::Instantiate(RESOURCE.GetPrefab("HP_Background"), owner->GetTransform());
	if (healthBar) {
		healthBar->GetTransform()->SetLocalPosition({ 0.f, 2.f, 0.f });
		auto healthSystem = healthBar->AddComponent<CHealthSystem>();
		healthSystem->SetRenderToWorld(true);
		healthSystem->SetHealthBarColor({ 0.8f, 0.f, 0.f, 1.f });
		healthSystem->BindOwner(healthBar);
		healthBar->SetActive(true);

		mHealthSystem = healthSystem;
	}
}

void CEnemyState::Update()
{
	auto transform = GetTransform();
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	MONSTER_STATE state = (MONSTER_STATE)currentState;
	switch (state)
	{
	case MONSTER_STATE::IDLE:
		break;
	case MONSTER_STATE::SPAWN:
		break;
	case MONSTER_STATE::UNDERGROUND:
		break;
	case MONSTER_STATE::RUN:
		break;
	case MONSTER_STATE::ATTACK:
		break;
	case MONSTER_STATE::PROJECTILE_ATTACK:
		break;
	case MONSTER_STATE::GETHIT:
		transform->SetHitFactor(1.f - controller->mTrack->mTrackProgress);
		break;
	case MONSTER_STATE::DEATH:
		break;
	case MONSTER_STATE::end:
		break;
	default:
		break;
	}
}

void CEnemyState::OnEnterState(UINT8 state)
{
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	switch ((MONSTER_STATE)state)
	{
	case MONSTER_STATE::IDLE:
	case MONSTER_STATE::RUN:
		break;
	case MONSTER_STATE::SPAWN:
		if (mHealthSystem.lock()) mHealthSystem.lock()->ViewHealthBar(false);
		mIsDead = false;
		mIsSpawning = true;
		mIsSpawningFinished = false;
		break;
	case MONSTER_STATE::UNDERGROUND:
		if (mHealthSystem.lock()) mHealthSystem.lock()->ViewHealthBar(false);
		break;
	case MONSTER_STATE::ATTACK:
	case MONSTER_STATE::PROJECTILE_ATTACK:
		break;
	case MONSTER_STATE::DEATH:
		mIsDead = true;
		break;
	case MONSTER_STATE::end:
		break;
	case MONSTER_STATE::GETHIT:
		mIsHit = true;
		GetTransform()->SetHitFactor(1.f);
		break;
	default:
		break;
	}
}

void CEnemyState::OnExitState(UINT8 state)
{
	switch ((MONSTER_STATE)state)
	{
	case MONSTER_STATE::IDLE:
		break;
	case MONSTER_STATE::SPAWN:
		mIsSpawning = false;
		mIsSpawningFinished = true;
		if(mHealthSystem.lock()) mHealthSystem.lock()->ViewHealthBar(true);
		break;
	case MONSTER_STATE::UNDERGROUND:
		break;
	case MONSTER_STATE::RUN:
		break;
	case MONSTER_STATE::ATTACK:
		break;
	case MONSTER_STATE::PROJECTILE_ATTACK:
		break;
	case MONSTER_STATE::GETHIT:
		mIsHit = false;
		GetTransform()->SetHitFactor(0.f);
		break;
	case MONSTER_STATE::DEATH:
		break;
	case MONSTER_STATE::end:
		break;
	default:
		break;
	}
}

void CGrassSmallState::Awake()
{
	CEnemyState::Awake();
	auto healthSystem = owner->GetComponentFromHierarchy<CHealthSystem>();
	mHealth = 50.f;
	mMaxHealth = 50.f;
}

void CGrassSmallState::Start()
{
	CEnemyState::Start();
	mAnimationController = owner->GetComponentFromHierarchy<CAnimationController>();

	auto healthSystem = mHealthSystem.lock();
	if (healthSystem) {
		healthSystem->SetMaxHealth(mHealth);
		healthSystem->SetHealth(mHealth);
		healthSystem->SetHealthBarScale({ mHealth / MAX_HEALTH, 0.7f });

		auto transform = healthSystem->GetTransform();
		if (transform) {
			transform->SetLocalPosition({ 0.f, 2.f, 0.f });
		}
	}
}

void CGrassSmallState::Update()
{
	CEnemyState::Update();
}

void CGrassSmallState::OnEnterState(UINT8 state)
{
	CEnemyState::OnEnterState(state);
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	controller->SetTrackAnimationSet((int)state);

}

void CGrassSmallState::OnExitState(UINT8 state)
{
	CEnemyState::OnExitState(state);
}

void CGrassBigState::Awake()
{
	CEnemyState::Awake();
	mHealth = 100.f;
	mMaxHealth = 100.f;
}

void CGrassBigState::Start()
{
	CEnemyState::Start();
	mAnimationController = owner->GetComponentFromHierarchy<CAnimationController>();

	auto healthSystem = mHealthSystem.lock();
	if (healthSystem) {
		healthSystem->SetMaxHealth(mHealth);
		healthSystem->SetHealth(mHealth);
		healthSystem->SetHealthBarScale({ mHealth / MAX_HEALTH, 1.f });

		auto transform = healthSystem->GetTransform();
		if (transform) {
			transform->SetLocalPosition({ 0.f, 4.f, 0.f });
		}
	}
}

void CGrassBigState::Update()
{
	CEnemyState::Update();
}

void CGrassBigState::OnEnterState(UINT8 state)
{
	CEnemyState::OnEnterState(state);
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	controller->SetTrackAnimationSet((int)state);

}

void CGrassBigState::OnExitState(UINT8 state)
{
	CEnemyState::OnExitState(state);
}
