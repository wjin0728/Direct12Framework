#include "stdafx.h"
#include "LoadingScreen.h"
#include "ResourceManager.h"
#include "UIRenderer.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include"Transform.h"
#include"Timer.h"

CLoadingScreen::CLoadingScreen()
{
}

void CLoadingScreen::Awake()
{
}

void CLoadingScreen::Start()
{
	mLoadingScreenIndex = 0;
	renderer = owner->GetComponentFromHierarchy<CUIRenderer>();
	mIsLoading = true;
}

void CLoadingScreen::Update()
{
	mTime += TIMER.GetDeltaTime();

	if (mTime > mLoadingScreenSpeed) {
		mLoadingScreenIndex = (mLoadingScreenIndex + 1) % mLoadingScreenTextureIdx.size();
		renderer->SetTextureIdx(mLoadingScreenTextureIdx[mLoadingScreenIndex]);
		mTime = 0.f;
	}
	if (!mIsLoading) {
		mFadeOutTime -= TIMER.GetDeltaTime();
		renderer->SetAlpha(mFadeOutTime);

		if (mFadeOutTime <= 0.f) {
			mFadeOutTime = 0.f;
			INSTANCE(CSceneManager).RequestSceneChange(SCENE_TYPE::MAINSTAGE2, false);
		}
	}
}

void CLoadingScreen::LateUpdate()
{
}
