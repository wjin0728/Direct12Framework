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
	controller->AddAnimationEvent("Run", " Dust", func);
	controller->AddAnimationEvent("RunAttack", " Dust", func);
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
		case PLAYER_STATE::RUNATTACK:
		case PLAYER_STATE::SKILL:
		case PLAYER_STATE::ULTIMATE:
			SetState((UINT8)PLAYER_STATE::IDLE);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::IDLE);
			break;
		case PLAYER_STATE::DEATH:
			break;
		default:
			break;
		}
	}

	if (mShieldDuration > 0) {
		mShieldDuration -= DELTA_TIME;
		if (mShieldDuration <= 0) {
			ActivateShield(false);
			mShieldDuration = -1.f;
		}
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
		break;
	case PLAYER_STATE::DEATH:
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
		case PLAYER_STATE::RUNATTACK:
		case PLAYER_STATE::SKILL:
		case PLAYER_STATE::ULTIMATE:
			SetState((UINT8)PLAYER_STATE::IDLE);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::IDLE);
			break;
		case PLAYER_STATE::DEATH:
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
	auto socket = owner->AddBoneSocket("Equipment.weapon.R.001", "WeaponSocket");
	if (socket) {
		mTrail = socket->AddComponent<CTrailRenderer>();
		mTrail.lock()->mIsActive = false;
		mAttackSocket = socket->GetTransform();
		mAttackSocket.lock()->SetLocalPosition(Vec3(0.0f, 0.5f, 0.0f));
	}
}

void CWarriorState::Start()
{
	CPlayerStateMachine::Start();
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
		case PLAYER_STATE::RUNATTACK:
		case PLAYER_STATE::SKILL:
		case PLAYER_STATE::ULTIMATE:
			SetState((UINT8)PLAYER_STATE::IDLE);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::IDLE);
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
			trail->mIsActive = true;
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
			trail->mIsActive = false;
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
			trail->mIsActive = true;
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
			trail->mIsActive = false;
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
			trail->mIsActive = false;
		}
	}
	break;
	default:
		break;
	}
}

void CMageState::Awake()
{
}

void CMageState::Start()
{
}

void CMageState::Update()
{
}

void CMageState::OnEnterState(UINT8 state)
{
}

void CMageState::OnExitState(UINT8 state)
{
}