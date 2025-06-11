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
		case (UINT8)PLAYER_STATE::JUMP:
		case (UINT8)PLAYER_STATE::FALLING:
		case (UINT8)PLAYER_STATE::GATHERING:
		case (UINT8)PLAYER_STATE::GETHIT:
		case (UINT8)PLAYER_STATE::ATTACK:
		case (UINT8)PLAYER_STATE::RUNATTACK:
		case (UINT8)PLAYER_STATE::SKILL:
		case (UINT8)PLAYER_STATE::ULTIMATE:
			SetState((UINT8)PLAYER_STATE::IDLE);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::IDLE);
			break;
		case (UINT8)PLAYER_STATE::DEATH:
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

	controller->SetTrackAnimationSet(0, (int)state);

	switch ((PLAYER_STATE)state) {
	case PLAYER_STATE::IDLE:
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::ATTACK:
	case PLAYER_STATE::RUNATTACK:
		controller->SetTrackSpeed(0, 3.0f);
		break;
	case PLAYER_STATE::GETHIT:
	case PLAYER_STATE::DEATH:
	case PLAYER_STATE::JUMP:
	case PLAYER_STATE::SKILL:
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

void CWarriorState::Awake()
{
}

void CWarriorState::Start()
{
}

void CWarriorState::Update()
{
}

void CWarriorState::OnEnterState(UINT8 state)
{
}

void CWarriorState::OnExitState(UINT8 state)
{
}
