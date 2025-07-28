#include "stdafx.h"
#include "EnemyState.h"
#include "Animation.h"
#include"Transform.h"
#include"ResourceManager.h"
#include"HealthSystem.h"
#include "Timer.h"
#include"Camera.h"
#include"RenderManager.h"
#include "GameObject.h"
#include"ParticleManager.h"

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
		healthSystem->ViewHealthBar(false);
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

	if (mIsHit) {
		mHitProgress += DELTA_TIME;
		float hitFactor = 1.f - (mHitProgress / 0.666f);
		if (hitFactor >= 0.f) {
			GetTransform()->SetHitFactor(hitFactor);
		}
		else {
			mHitProgress = -ANIMATION_CALLBACK_EPSILON;
			SetHit(false);
		}
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
	case MONSTER_STATE::SKILL:
		break;
	case MONSTER_STATE::GETHIT:
	{
		break;
	}
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
		GetTransform()->SetHitFactor(0.f);
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
	case MONSTER_STATE::SKILL:
		GetTransform()->SetHitFactor(0.f);
		break;
	case MONSTER_STATE::DEATH:
		mIsDead = true;
		break;
	case MONSTER_STATE::end:
		break;
	case MONSTER_STATE::GETHIT:
		SetHit(true);
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
	case MONSTER_STATE::SKILL:
		break;
	case MONSTER_STATE::GETHIT:
		GetTransform()->SetHitFactor(0.f);
		break;
	case MONSTER_STATE::DEATH:
		if (mHealthSystem.lock()) mHealthSystem.lock()->ViewHealthBar(false);
		break;
	case MONSTER_STATE::end:
		break;
	default:
		break;
	}
}

void CEnemyState::SetHitFactor(float hitFactor)
{
	auto transform = GetTransform();
	transform->SetHitFactor(hitFactor);
}

/// <summary>
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// </summary>

void CGrassSmallState::Awake()
{
	CEnemyState::Awake();
	auto healthSystem = owner->GetComponentFromHierarchy<CHealthSystem>();
	mHealth = mMaxHealth = MAX_HEALTH / 2.f;
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

/// <summary>
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// </summary>

void CGrassBigState::Awake()
{
	CEnemyState::Awake();
	mHealth = mMaxHealth = MAX_HEALTH;
}

void CGrassBigState::Start()
{
	CEnemyState::Start();
	mAnimationController = owner->GetComponentFromHierarchy<CAnimationController>();

	mAttackPoint = owner->FindChildByName("RigLArmPalm")->GetTransform();
	auto func1 = [this](float time) {
		if (auto attackPoint = mAttackPoint.lock()) {
			auto transform = attackPoint->GetTransform();
			if (transform) {
				Vec3 position = transform->GetWorldPosition();
				Vec3 forward = transform->GetWorldLook();
				position -= forward * 0.4f;
				INSTANCE(CParticleManager).PlayParticleEmitter(
					"MonsterAttack",
					position
				);
			}
		}
		};
	mAnimationController.lock()->AddAnimationEvent("Attack", "Attack", func1);

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

/// <summary>
/// ////////////////////////////////////////////////////////////////////////
/// </summary>

void CWaterBigState::Awake()
{
	CEnemyState::Awake();
	mHealth = mMaxHealth = MAX_HEALTH;
}

void CWaterBigState::Start()
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


void CWaterBigState::Update()
{
	CEnemyState::Update();
}

void CWaterBigState::OnEnterState(UINT8 state)
{
	CEnemyState::OnEnterState(state);
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	controller->SetTrackAnimationSet((int)state);

}

void CWaterBigState::OnExitState(UINT8 state)
{
	CEnemyState::OnExitState(state);
}

/// <summary>
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// </summary>

void CWaterSmallState::Awake()
{
	CEnemyState::Awake();
	mHealth = mMaxHealth = MAX_HEALTH / 2.f;
}

void CWaterSmallState::Start()
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
			transform->SetLocalPosition({ 0.f, 3.f, 0.f });
		}
	}
}


void CWaterSmallState::Update()
{
	CEnemyState::Update();
}

void CWaterSmallState::OnEnterState(UINT8 state)
{
	CEnemyState::OnEnterState(state);
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	controller->SetTrackAnimationSet((int)state);

}

void CWaterSmallState::OnExitState(UINT8 state)
{
	CEnemyState::OnExitState(state);
}

/// <summary>
/// ////////////////////////////////////////////////////////////////////////////////////////
/// 
/// </summary>
/// 
void CFireBigState::Awake()
{
	CEnemyState::Awake();
	mHealth = mMaxHealth = MAX_HEALTH;
}

void CFireBigState::Start()
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


void CFireBigState::Update()
{
	CEnemyState::Update();
}

void CFireBigState::OnEnterState(UINT8 state)
{
	CEnemyState::OnEnterState(state);
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	controller->SetTrackAnimationSet((int)state);

}

void CFireBigState::OnExitState(UINT8 state)
{
	CEnemyState::OnExitState(state);
}

/// <summary>
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// </summary>

void CFireSmallState::Awake()
{
	CEnemyState::Awake();
	mHealth = mMaxHealth = MAX_HEALTH / 2.f;
}

void CFireSmallState::Start()
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
			transform->SetLocalPosition({ 0.f, 3.f, 0.f });
		}
	}
}

void CFireSmallState::Update()
{
	CEnemyState::Update();
}


void CFireSmallState::OnEnterState(UINT8 state)
{
	CEnemyState::OnEnterState(state);
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	controller->SetTrackAnimationSet((int)state);
}

void CFireSmallState::OnExitState(UINT8 state)
{
	CEnemyState::OnExitState(state);
}

/// <summary>
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// </summary>

void CBossState::Awake()
{
	CEnemyState::Awake();
	mHealth = mMaxHealth = MAX_HEALTH * 3.f;
}

void CBossState::Start()
{
	CEnemyState::Start();
	mAnimationController = owner->GetComponentFromHierarchy<CAnimationController>();
	auto healthSystem = mHealthSystem.lock();
	if (healthSystem) {
		healthSystem->SetMaxHealth(mHealth);
		healthSystem->SetHealth(mHealth);
		healthSystem->SetHealthBarScale({ mHealth / MAX_HEALTH, 2.f });
		auto transform = healthSystem->GetTransform();
		if (transform) {
			transform->SetLocalPosition({ 0.f, 4.f, 0.f });
		}
	}
}

void CBossState::Update()
{
	CEnemyState::Update();
}

void CBossState::OnEnterState(UINT8 state)
{
	CEnemyState::OnEnterState(state);
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	controller->SetTrackAnimationSet((int)state);
}

void CBossState::OnExitState(UINT8 state)
{
	CEnemyState::OnExitState(state);
}
