#include "stdafx.h"
#include "EnemyState.h"
#include "Animation.h"
#include "Transform.h"

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
		transform->SetHitFactor(1.f - controller->mTracks.front()->mTrackProgress);
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
		controller->SetTrackType(0, ANIMATION_TYPE::LOOP);
		break;
	case MONSTER_STATE::SPAWN:
	case MONSTER_STATE::UNDERGROUND:
	case MONSTER_STATE::ATTACK:
	case MONSTER_STATE::PROJECTILE_ATTACK:
	case MONSTER_STATE::DEATH:
		controller->SetTrackType(0, ANIMATION_TYPE::ONCE);
		break;
	case MONSTER_STATE::end:
		break;
	case MONSTER_STATE::GETHIT:
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

void CGrassSmallState::Start()
{
}

void CGrassSmallState::Update()
{
	CEnemyState::Update();
}

void CGrassSmallState::OnEnterState(UINT8 state)
{
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	controller->SetTrackAnimationSet(0, (int)state);
	controller->SetTrackSpeed(0, 1.0f);
	controller->SetTrackWeight(0, 1.0f);
	controller->SetTrackPosition(0, 0.0f);

	CEnemyState::OnEnterState(state);
}

void CGrassSmallState::OnExitState(UINT8 state)
{
	CEnemyState::OnExitState(state);
}

void CGrassBigState::Start()
{
}

void CGrassBigState::Update()
{
	CEnemyState::Update();
}

void CGrassBigState::OnEnterState(UINT8 state)
{
	auto controller = mAnimationController.lock();
	if (!controller) {
		return;
	}
	controller->SetTrackAnimationSet(0, (int)state);
	controller->SetTrackSpeed(0, 1.0f);
	controller->SetTrackWeight(0, 1.0f);
	controller->SetTrackPosition(0, 0.0f);

	CEnemyState::OnEnterState(state);
}

void CGrassBigState::OnExitState(UINT8 state)
{
	CEnemyState::OnExitState(state);
}
