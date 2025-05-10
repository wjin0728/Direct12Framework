#include "stdafx.h"
#include "ItemMovement.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include"Camera.h"

void CItemMovement::Awake()
{
}

void CItemMovement::Start()
{
	auto transform = GetTransform();
	if (mDirection == Vec3(0.f, 0.f, 0.f)) {
		mDirection = transform->GetLocalUp();
	}
	if (mStartPos == Vec3(0.f, 0.f, 0.f)) {
		mStartPos = transform->GetLocalPosition();
	}
}

void CItemMovement::Update()
{
	auto transform = GetTransform();
	Vec3 pos = mStartPos;
	pos += mDirection * mAmplitude * sinf(TIMER.GetTotalTime() * mFrequency);
	transform->SetLocalPosition(pos);
	if (mTargetObject) {
		Vec3 targetlook = mTargetObject->GetTransform()->GetLocalLook();
		transform->LookTo(-targetlook);
	}
}

void CItemMovement::LateUpdate()
{
}
