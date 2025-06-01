#include "stdafx.h"
#include "ObjectState.h"
#include "PlayerController.h"
#include "Animation.h"
#include "ServerManager.h"

void CPlayerStateMachine::Awake()
{

}

void CPlayerStateMachine::Start()
{
	mAnimationController = owner->GetComponentFromHierarchy<CAnimationController>();
}

void CPlayerStateMachine::Update()
{
	auto controller = mAnimationController.lock();
	if(!controller) {
		return;
	}
	if (controller->mTracks.front()->mType == ANIMATION_TYPE::END) {
		switch (currentState) {
		case (UINT8)PLAYER_STATE::IDLE:
		case (UINT8)PLAYER_STATE::RUN:
			break;
		case (UINT8)PLAYER_STATE::ATTACK:
		case (UINT8)PLAYER_STATE::MOVE_ATTACK:
		case (UINT8)PLAYER_STATE::GETHIT:
		case (UINT8)PLAYER_STATE::DEATH:
		case (UINT8)PLAYER_STATE::JUMP:
		case (UINT8)PLAYER_STATE::SKILL:
			SetState((UINT8)PLAYER_STATE::IDLE);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::IDLE);
			break;
		default:
			break;
		}
	}
}

void CPlayerStateMachine::OnEnterState(UINT8 state)
{
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}

	switch (mClass) {
	case PLAYER_CLASS::ARCHER:
		controller->SetTrackAnimationSet(0, (int)CAnimationController::ARCHER_MAP[(PLAYER_STATE)state]);
		break;
	case PLAYER_CLASS::FIGHTER:
		controller->SetTrackAnimationSet(0, (int)CAnimationController::FIGHTER_MAP[(PLAYER_STATE)state]);
		break;
	case PLAYER_CLASS::MAGE:
		controller->SetTrackAnimationSet(0, (int)CAnimationController::MAGE_MAP[(PLAYER_STATE)state]);
		break;
	default:
		break;
	}

	controller->SetTrackSpeed(0, 1.0f);
	controller->SetTrackWeight(0, 1.0f);
	controller->SetTrackPosition(0, 0.0f);

	switch ((PLAYER_STATE)state) {
	case PLAYER_STATE::IDLE:
	case PLAYER_STATE::RUN:
		controller->SetTrackType(0, ANIMATION_TYPE::LOOP);
		break;
	case PLAYER_STATE::ATTACK:
	case PLAYER_STATE::MOVE_ATTACK:
	case PLAYER_STATE::GETHIT:
	case PLAYER_STATE::DEATH:
	case PLAYER_STATE::JUMP:
	case PLAYER_STATE::SKILL:
		controller->SetTrackType(0, ANIMATION_TYPE::ONCE);
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
	case PLAYER_STATE::MOVE_ATTACK:
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
}

void CArcherState::Start()
{
}

void CArcherState::Update()
{
}

void CArcherState::OnEnterState(UINT8 state)
{
}

void CArcherState::OnExitState(UINT8 state)
{
}
