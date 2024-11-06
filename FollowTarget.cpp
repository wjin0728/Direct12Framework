#include "stdafx.h"
#include "FollowTarget.h"
#include"GameObject.h"
#include"Transform.h"
#include"Timer.h"

CFollowTarget::CFollowTarget() : CMonoBehaviour("FollowTarget")
{
}

CFollowTarget::~CFollowTarget()
{
}

void CFollowTarget::Awake()
{
}

void CFollowTarget::Start()
{
}

void CFollowTarget::Update()
{
	auto targetTrans = mTarget->GetTransform();
	auto transform = GetTransform();

	Vec3 dir = targetTrans->GetLocalLook();
	dir.y = 0.f;
	if (dir.Length() > 0) dir.Normalize();

	float f = DELTA_TIME * 20.f;

	Vec3 position = Vec3::Lerp(transform->GetLocalPosition(), targetTrans->GetLocalPosition(), f);
	transform->SetLocalPosition(position);

	Quaternion rotation = Quaternion::Slerp(transform->GetLocalRotation(), Quaternion::LookRotation(dir), f);
	transform->SetLocalRotation(rotation);
}

void CFollowTarget::LateUpdate()
{
}
