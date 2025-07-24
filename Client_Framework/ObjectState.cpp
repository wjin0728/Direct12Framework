#include "stdafx.h"
#include "ObjectState.h"
#include "PlayerController.h"
#include "Animation.h"
#include "ServerManager.h"
#include "CutScene.h"
#include"Timer.h"
#include"TrailRenderer.h"
#include"Transform.h"
#include"ResourceManager.h"
#include"ParticleManager.h"
#include"ParticleAttach.h"

void CPlayerStateMachine::Awake()
{

}

void CPlayerStateMachine::Start()
{
	mAnimationController = owner->GetComponentFromHierarchy<CAnimationController>();
	auto controller = mAnimationController.lock();
	auto func = [this](float time) {
		INSTANCE(CParticleManager).PlayParticleEmitter(
			"FootDust",
			owner->GetTransform()->GetWorldMat()
		);
		};
	controller->AddAnimationEvent("Run", "Dust", func);
	controller->AddAnimationEvent("RunAttack", "Dust", func);

	if (mClass == PLAYER_CLASS::FIGHTER) {
		mHealth = mMaxHealth = MAX_HP_FIGHTER;
	}
	else {
		mHealth = mMaxHealth = MAX_HP_ARCHER_MAGE;
	}
	ActivateShield(false);

}

void CPlayerStateMachine::Update()
{
	auto controller = mAnimationController.lock();
	if(!controller) {
		return;
	}
	if (controller->mTrack->mType == ANIMATION_TYPE::END) {
		switch ((PLAYER_STATE)currentState) {
		case PLAYER_STATE::JUMP:
		case PLAYER_STATE::FALLING:
		case PLAYER_STATE::GATHERING:
		case PLAYER_STATE::GETHIT:
		case PLAYER_STATE::ATTACK:
		case PLAYER_STATE::SKILL:
		case PLAYER_STATE::ULTIMATE:
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::IDLE);
			break;
		case PLAYER_STATE::DEATH:
			break;
		case PLAYER_STATE::RUNATTACK:
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::RUN);
			break;
		default:
			break;
		}
	}

	switch ((PLAYER_STATE)currentState) {
	case PLAYER_STATE::JUMP:
	case PLAYER_STATE::FALLING:
	case PLAYER_STATE::GATHERING:
	case PLAYER_STATE::GETHIT:
	{
		float hitFactor = 1.f - (controller->mTrack->mTrackProgress * 4);
		if (hitFactor < 0.f) hitFactor = 0.f;
		GetTransform()->SetHitFactor(hitFactor);
	}
		break;
	case PLAYER_STATE::ATTACK:
	case PLAYER_STATE::SKILL:
	case PLAYER_STATE::ULTIMATE:
		break;
	case PLAYER_STATE::DEATH:
		break;
	case PLAYER_STATE::RUNATTACK:
		break;
	default:
		break;
	}

	if (mShieldDuration > 0) {
		mShieldDuration -= DELTA_TIME;
		if (mShieldDuration <= 0) {
			ActivateShield(false);
		}
	}
	else if (mShieldDuration <= 0) {
		ActivateShield(false);
	}
}

void CPlayerStateMachine::OnEnterState(UINT8 state)
{
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}

	controller->SetTrackAnimationSet((int)state);
	auto cutscene = owner->GetComponentFromHierarchy<CCutScene>();

	switch ((PLAYER_STATE)state) {
	case PLAYER_STATE::IDLE:
		break;
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::ATTACK:
		break;
	case PLAYER_STATE::RUNATTACK:
		break;
	case PLAYER_STATE::GETHIT:
		mIsHit = true;
		mIsInvisible = true;
		GetTransform()->SetHitFactor(1.f);
		break;
	case PLAYER_STATE::DEATH:
		mIsHit = false;
		mIsDead = true;
		break;
	case PLAYER_STATE::JUMP:
		break;
	case PLAYER_STATE::SKILL:
		break;
	case PLAYER_STATE::ULTIMATE:
		if (cutscene && !cutscene->GetEnable())
			cutscene->PlayCutScene();
		break;
	default:
		break;
	}
}

void CPlayerStateMachine::OnExitState(UINT8 state)
{
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}

	switch ((PLAYER_STATE)state) {
	case PLAYER_STATE::IDLE:
		break;
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::ATTACK:
		break;
	case PLAYER_STATE::RUNATTACK:
		break;
	case PLAYER_STATE::GETHIT:
		mIsInvisible = false;
		GetTransform()->SetHitFactor(0.f);
		mIsHit = false;
		break;
	case PLAYER_STATE::DEATH:
		break;
	case PLAYER_STATE::JUMP:
		break;
	case PLAYER_STATE::SKILL:
		break;
	default:
		break;
	}
}

void CPlayerStateMachine::GetHit(float damage)
{
	CEntityState::GetHit(damage);
	float hpRatio = mHealth / mMaxHealth;
	owner->TriggerEvent("OnFaceChanged", { hpRatio >= 0.3f });
}

void CPlayerStateMachine::Heal(float amount)
{
}

void CPlayerStateMachine::UpdateHealth(float newHealth, int newSheild)
{
	CEntityState::UpdateHealth(newHealth);
	float hpRatio = mHealth / mMaxHealth;
	owner->TriggerEvent("OnFaceChanged", { hpRatio >= 0.3f });

	mShieldHealth = newSheild;
	std::cout << "HP: " << mHealth << ", 남은 실드: " << mShieldHealth << std::endl;
}

void CArcherState::Awake()
{
	CPlayerStateMachine::Awake();
}

void CArcherState::Start()
{
	CPlayerStateMachine::Start();
}

void CArcherState::Update()
{
	CPlayerStateMachine::Update();
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	if (controller->mTrack->mType == ANIMATION_TYPE::END) {
		switch ((PLAYER_STATE)currentState) {
		case PLAYER_STATE::JUMP:
		case PLAYER_STATE::FALLING:
		case PLAYER_STATE::GATHERING:
		case PLAYER_STATE::GETHIT:
		case PLAYER_STATE::ATTACK:
		case PLAYER_STATE::SKILL:
		case PLAYER_STATE::ULTIMATE:
			break;
		case PLAYER_STATE::DEATH:
			break;
		case PLAYER_STATE::RUNATTACK:
		break;
		default:
			break;
		}
	}
}

void CArcherState::OnEnterState(UINT8 state)
{
	CPlayerStateMachine::OnEnterState(state);
}

void CArcherState::OnExitState(UINT8 state)
{
	CPlayerStateMachine::OnExitState(state);
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	switch ((PLAYER_STATE)state) {
	case PLAYER_STATE::IDLE:
		break;
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::ATTACK:
		break;
	case PLAYER_STATE::RUNATTACK:
		break;
	case PLAYER_STATE::GETHIT:
		break;
	case PLAYER_STATE::DEATH:
		break;
	case PLAYER_STATE::JUMP:
		break;
	case PLAYER_STATE::SKILL:
		break;
	default:
		break;
	}
}

void CWarriorState::Awake()
{
	CPlayerStateMachine::Awake();
}

void CWarriorState::Start()
{
	CPlayerStateMachine::Start();
	auto socket = owner->AddBoneSocket("Equipment.weapon.R.001", "WeaponSocket");
	if (socket) {
		mTrail = socket->AddComponent<CTrailRenderer>();
		mTrail.lock()->mActive = false;
		mAttackSocket = socket->GetTransform();
		mAttackSocket.lock()->SetLocalPosition(Vec3(0.0f, 0.6f, 0.0f));
		socket->SetRenderer(mTrail.lock());
		socket->SetActive(true);
	}
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	if (controller->mTrack->mType == ANIMATION_TYPE::END) {
		switch ((PLAYER_STATE)currentState) {
		case PLAYER_STATE::JUMP:
		case PLAYER_STATE::FALLING:
		case PLAYER_STATE::GATHERING:
		case PLAYER_STATE::GETHIT:
		case PLAYER_STATE::ATTACK:
		case PLAYER_STATE::RUNATTACK:
		case PLAYER_STATE::SKILL:
		case PLAYER_STATE::ULTIMATE:
			break;
		case PLAYER_STATE::DEATH:
			break;
		default:
			break;
		}
	}
	auto func0 = [this](float time) {
		if (mTrail.expired()) {
			return;
		}
		auto trail = mTrail.lock();
		if (trail) {
			trail->mActive = true;
			trail->ResetTrail();
			trail->SetDuration(0.2f);
		}
		};
	auto func1 = [this](float time) {
		if (mTrail.expired()) {
			return;
		}
		auto trail = mTrail.lock();
		if (trail) {
			trail->mActive = false;
		}
		};
	controller->AddAnimationEvent("Attack", "AttackStart", func0);
	controller->AddAnimationEvent("Attack", "AttackEnd", func1);
	controller->AddAnimationEvent("RunAttack", "AttackStart", func0);
	controller->AddAnimationEvent("RunAttack", "AttackEnd", func1);

	auto trail = mTrail.lock();
	if (trail) {
		trail->SetBlendMaskTexture("WeaponTrail");
	}
}

void CWarriorState::Update()
{
	CPlayerStateMachine::Update();
}

void CWarriorState::OnEnterState(UINT8 state)
{
	CPlayerStateMachine::OnEnterState(state);
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	switch ((PLAYER_STATE)state) {
	case PLAYER_STATE::IDLE:
		break;
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::ATTACK:
		break;
	case PLAYER_STATE::RUNATTACK:
	{
	}
	break;
	case PLAYER_STATE::GETHIT:
		break;
	case PLAYER_STATE::DEATH:
		break;
	case PLAYER_STATE::JUMP:
		break;
	case PLAYER_STATE::SKILL:
		break;
	case PLAYER_STATE::ULTIMATE:
	{
		auto trail = mTrail.lock();
		if (trail) {
			trail->mActive = true;
			trail->ResetTrail();
			trail->SetDuration(1.0f);
		}
	}
	break;
	default:
		break;
	}
}

void CWarriorState::OnExitState(UINT8 state)
{
	CPlayerStateMachine::OnExitState(state);
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	switch ((PLAYER_STATE)state) {
	case PLAYER_STATE::IDLE:
		break;
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::ATTACK:
		break;
	case PLAYER_STATE::RUNATTACK:
	{
		auto trail = mTrail.lock();
		if (trail) {
			trail->mActive = false;
		}
	}
		break;
	case PLAYER_STATE::GETHIT:
		break;
	case PLAYER_STATE::DEATH:
		break;
	case PLAYER_STATE::JUMP:
		break;
	case PLAYER_STATE::SKILL:
		break;
	case PLAYER_STATE::ULTIMATE:
	{
		auto trail = mTrail.lock();
		if (trail) {
			trail->mActive = false;
		}
	}
	break;
	default:
		break;
	}
}

void CMageState::Awake()
{
	CPlayerStateMachine::Awake();
	mClass = PLAYER_CLASS::MAGE;
	mMaxHealth = MAX_HP_ARCHER_MAGE;
	mHealth = mMaxHealth;
	mShieldHealth = 0;
	mShieldDuration = 0.f;
	mIsDead = false;
	mIsHit = false;
}

void CMageState::Start()
{
	CPlayerStateMachine::Start();
	mStaffParticle = owner->GetComponentFromHierarchy<CParticleAttach>();

	auto func0 = [this](float time) {
		if (mStaffParticle.expired()) {
			return;
		}
		auto staff = mStaffParticle.lock();
		if (staff) {
			staff->Play();
		}
		};

	auto controller = mAnimationController.lock();

	controller->AddAnimationEvent("Attack", "Attack", func0);
	controller->AddAnimationEvent("RunAttack", "Attack", func0);
	if (!controller) {
		return;
	}
	if (controller->mTrack->mType == ANIMATION_TYPE::END) {
		switch ((PLAYER_STATE)currentState) {
		case PLAYER_STATE::JUMP:
		case PLAYER_STATE::FALLING:
		case PLAYER_STATE::GATHERING:
		case PLAYER_STATE::GETHIT:
		case PLAYER_STATE::ATTACK:
		case PLAYER_STATE::SKILL:
		case PLAYER_STATE::ULTIMATE:
			break;
		case PLAYER_STATE::DEATH:
			break;
		case PLAYER_STATE::RUNATTACK:
			break;
		default:
			break;
		}
	}
}

void CMageState::Update()
{
	CPlayerStateMachine::Update();
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	if (controller->mTrack->mType == ANIMATION_TYPE::END) {
		switch ((PLAYER_STATE)currentState) {
		case PLAYER_STATE::JUMP:
		case PLAYER_STATE::FALLING:
		case PLAYER_STATE::GATHERING:
		case PLAYER_STATE::GETHIT:
		case PLAYER_STATE::ATTACK:
		case PLAYER_STATE::SKILL:
		case PLAYER_STATE::ULTIMATE:
			break;
		case PLAYER_STATE::DEATH:
			break;
		case PLAYER_STATE::RUNATTACK:
			break;
		default:
			break;
		}
	}
}

void CMageState::OnEnterState(UINT8 state)
{
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	controller->SetTrackAnimationSet((int)state);
	auto cutscene = owner->GetComponentFromHierarchy<CCutScene>();
	switch ((PLAYER_STATE)state) {
	case PLAYER_STATE::IDLE:
		break;
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::ATTACK:
		break;
	case PLAYER_STATE::RUNATTACK:
		break;
	case PLAYER_STATE::GETHIT:
		mIsHit = true;
		mIsInvisible = true;
		GetTransform()->SetHitFactor(1.f);
		break;
	case PLAYER_STATE::DEATH:
		mIsHit = false;
		mIsDead = true;
		break;
	case PLAYER_STATE::JUMP:
		break;
	case PLAYER_STATE::SKILL:
		break;
	case PLAYER_STATE::ULTIMATE:
		if (cutscene && !cutscene->GetEnable())
			cutscene->PlayCutScene();
		break;
	default:
		break;
	}
}

void CMageState::OnExitState(UINT8 state)
{
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	switch ((PLAYER_STATE)state) {
	case PLAYER_STATE::IDLE:
		break;
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::ATTACK:
		break;
	case PLAYER_STATE::RUNATTACK:
		break;
	case PLAYER_STATE::GETHIT:
		mIsInvisible = false;
		GetTransform()->SetHitFactor(0.f);
		mIsHit = false;
		break;
	case PLAYER_STATE::DEATH:
		break;
	case PLAYER_STATE::JUMP:
		break;
	case PLAYER_STATE::SKILL:
		break;
	default:
		break;
	}
}
