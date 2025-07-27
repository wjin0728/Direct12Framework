#include "stdafx.h"
#include "Vine.h"
#include "GameObject.h"
#include "Transform.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include"Timer.h"

void CVine::Awake()
{
}

void CVine::Start()
{
	mOffset = 1.f;
	auto transform = GetTransform();
	mMaxPosY = transform->GetLocalPosition().y;
	transform->MoveUp(-mOffset);
	mMinPosY = transform->GetLocalPosition().y;

	mMinRotationY = 0;
	mMaxRotationY = 360.f;

	mTotalDuration = 3.f;
	mTransitionDuration = 0.5f;
	mTime = 0.f;
	mMinScale = { 0.03f, 0.0068f, 0.023f };
	mMaxScale = { 0.227f, 0.0476f, 0.160f };


}

void CVine::Update()
{
	if (mTime < mTotalDuration)
	{
		mTime += DELTA_TIME;
	}
	else
	{
		INSTANCE(CSceneManager).GetCurScene()->DestroyObject(owner);
		return;
	}
	auto transform = GetTransform();

	if(mTime <= mTransitionDuration) {
		float t = mTime / mTransitionDuration;
		float scaleX = mMinScale.x + (mMaxScale.x - mMinScale.x) * t;
		float scaleY = mMinScale.y + (mMaxScale.y - mMinScale.y) * t;
		float scaleZ = mMinScale.z + (mMaxScale.z - mMinScale.z) * t;
		transform->SetLocalScale({ scaleX, scaleY, scaleZ });

		float y = mMinPosY + (mMaxPosY - mMinPosY) * t;
		y = lerp(y, mMaxPosY, t);
		transform->SetLocalPositionY(y);
		float rotateY = mMinRotationY + (mMaxRotationY - mMinRotationY) * t;
		rotateY = lerp(rotateY, mMaxRotationY, t);
		transform->SetLocalRotationY(rotateY);
	}
	else if (mTime <= mTotalDuration - mTransitionDuration) {
		transform->SetLocalScale(mMaxScale);
	}
	else {
		float t = (mTime - (mTotalDuration - mTransitionDuration)) / mTransitionDuration;
		float scaleX = mMaxScale.x + (mMinScale.x - mMaxScale.x) * t;
		float scaleY = mMaxScale.y + (mMinScale.y - mMaxScale.y) * t;
		float scaleZ = mMaxScale.z + (mMinScale.z - mMaxScale.z) * t;
		transform->SetLocalScale({ scaleX, scaleY, scaleZ });

		float y = mMaxPosY + (mMinPosY - mMaxPosY) * t;
		y = lerp(y, mMinPosY, t);
		transform->SetLocalPositionY(y);
		float rotateY = mMaxRotationY + (mMinRotationY - mMaxRotationY) * t;
		rotateY = lerp(rotateY, mMinRotationY, t);
		transform->SetLocalRotationY(rotateY);
	}


}

void CVine::LateUpdate()
{
}
