#include "stdafx.h"
#include "ObjectState.h"
#include "PlayerController.h"
#include "Animation.h"

void CObjectStateMachine::Awake()
{

}

void CObjectStateMachine::Update()
{
	auto& controller = owner->GetComponentFromHierarchy<CAnimationController>();
	if (controller->mAnimationSets->mAnimationSet[controller->mTracks[0]->mIndex]->mType == ANIMATION_TYPE::END) {
		switch (currentState) {
		case PLAYER_STATE::IDLE:
		case PLAYER_STATE::RUN:
			break;
		case PLAYER_STATE::ATTACK:
		case PLAYER_STATE::MOVE_ATTACK:
		case PLAYER_STATE::GETHIT:
		case PLAYER_STATE::DEATH:
		case PLAYER_STATE::JUMP:
		case PLAYER_STATE::SKILL:
			SetState(PLAYER_STATE::IDLE);
			break;
		default:
			break;
		}
	}
}

void CObjectStateMachine::OnEnterState(PLAYER_STATE state)
{
	auto& controller = owner->GetComponentFromHierarchy<CAnimationController>();

	switch (mClass) {
	case PLAYER_CLASS::ARCHER:
		controller->SetTrackAnimationSet(0, (int)CAnimationController::ARCHER_MAP[state]);
		break;
	case PLAYER_CLASS::FIGHTER:
		controller->SetTrackAnimationSet(0, (int)CAnimationController::FIGHTER_MAP[state]);
		break;
	case PLAYER_CLASS::MAGE:
		controller->SetTrackAnimationSet(0, (int)CAnimationController::MAGE_MAP[state]);
		break;
	default:
		break;
	}

	controller->SetTrackSpeed(0, 1.0f);
	controller->SetTrackWeight(0, 1.0f);
	controller->SetTrackPosition(0, 0.0f);

	switch (state) {
	case PLAYER_STATE::IDLE:
	case PLAYER_STATE::RUN:
		controller->SetAnimationType(0, ANIMATION_TYPE::LOOP);
		break;
	case PLAYER_STATE::ATTACK:
	case PLAYER_STATE::MOVE_ATTACK:
	case PLAYER_STATE::GETHIT:
	case PLAYER_STATE::DEATH:
	case PLAYER_STATE::JUMP:
	case PLAYER_STATE::SKILL:
		controller->SetAnimationType(0, ANIMATION_TYPE::ONCE);
		break;
	default:
		break;
	}
}

void CObjectStateMachine::OnExitState(PLAYER_STATE state)
{
	auto& controller = owner->GetComponentFromHierarchy<CAnimationController>();

	switch (state) {
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
