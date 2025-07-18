#include "stdafx.h"
#include "TargetMarker.h"
#include "GameObject.h"
#include "Transform.h"
#include"UIRenderer.h"
#include"Camera.h"
#include "RenderManager.h"
#include"Timer.h"

CTargetMarker::CTargetMarker()
{
}

CTargetMarker::~CTargetMarker()
{
}

void CTargetMarker::Awake()
{
	mLockTime = 0.0f;
	mLockTimeMax = 0.2f;
	mLockTimeRate = 1 / mLockTimeMax;
	mTargetChanged = false;
}

void CTargetMarker::Start()
{
	mRenderer = owner->GetComponent<CUIRenderer>();
	mRenderer.lock()->mIsVisible = false;
}

void CTargetMarker::Update()
{
}

void CTargetMarker::LateUpdate()
{
	auto target = mTarget.lock();
	auto renderer = mRenderer.lock();
	if (target == nullptr) return;
	if (renderer == nullptr) return;

	auto camera = INSTANCE(CRenderManager).GetCamera("MainCamera");

	if (target) {
		BoundingSphere targetBS = target->GetRootBoundingSphere();
		renderer->SetPosition(camera->TransformToNDC(targetBS.Center));

		if (mLockTime < 1.f) {
			mLockTime += DELTA_TIME * mLockTimeRate;
		}
		mLockTime = std::min<float>(mLockTime, 1.f);
		float targetSize = lerp(mOriginalSize * 2, mOriginalSize, mLockTime);
		Color targetColor = Color::Lerp(Color(1.f,1.f,1.f,1.f), Color(1.f, 0.f, 0.f, 1.f), mLockTime);
		renderer->SetSize({ targetSize, targetSize });
		renderer->SetColor(targetColor);
	}
}

void CTargetMarker::SetTarget(std::shared_ptr<CGameObject> target)
{
	if(target == nullptr) {
		if(mRenderer.lock()) mRenderer.lock()->mIsVisible = false;
	}
	if(mTarget.lock() == target)
		return;
	mTarget = target;
	mTargetChanged = true;

	auto targetEnemy = mTarget.lock();
	auto renderer = mRenderer.lock();

	auto camera = INSTANCE(CRenderManager).GetCamera("MainCamera");
	if (targetEnemy) {
		renderer->mIsVisible = true;
		BoundingSphere targetBS = targetEnemy->GetRootBoundingSphere();
		Vec3 targetPos = targetBS.Center;
		Vec3 targetUp = targetBS.Center + camera->GetUp() * targetBS.Radius;
		Vec2 targetPosScreen = camera->TransformToScreenSpace(targetPos);
		Vec2 targetUpScreen = camera->TransformToScreenSpace(targetUp);
		float targetSize = abs((targetUpScreen - targetPosScreen).y);
		targetSize = std::min<float>(targetSize, 120.f);
		mOriginalSize = targetSize;
		mIsLocking = true;
		mLockTime = 0.0f;
	}
	else {
		renderer->mIsVisible = false;
	}
}
