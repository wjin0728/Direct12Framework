#include "stdafx.h"
#include "MentDisplay.h"
#include "GameObject.h"
#include "Transform.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include"RenderManager.h"
#include"UIRenderer.h"
#include"ResourceManager.h"
#include "Texture.h"
#include "Timer.h"



void CMentDisplay::Awake()
{

}

void CMentDisplay::Start()
{
	mUIRenderer = owner->GetComponent<CUIRenderer>();
	mAlpha = 1.0f;
	mTime = 0.0f;
	mFadeTime = 1.f;
	mDisplayTime = 4.f;
	mWaitTime = 0.5f;
	mCurrentTextureIndex = 0;

}

void CMentDisplay::Update()
{
    if (mTextures.empty()) {
        return; // No textures to display
	}
    float deltaTime = DELTA_TIME;
    mTime += deltaTime;

    if (mTime <= mFadeTime) {
        mAlpha = mTime / mFadeTime;
    }
    else if (mTime <= mDisplayTime - mFadeTime) {
        mAlpha = 1.0f;
    }
    else if (mTime <= mDisplayTime) {
        float fadeOutTime = mTime - (mDisplayTime - mFadeTime);
        mAlpha = 1.0f - (fadeOutTime / mFadeTime);
    }
    else if (mTime <= mDisplayTime + mWaitTime) {
        mAlpha = 0.0f;
    }
    else {
        mTime = 0.0f;
        mCurrentTextureIndex++;
        if (mCurrentTextureIndex >= mTextures.size()) {
            mCurrentTextureIndex = 0;
            owner->SetActive(false);
            return;
        }
        mUIRenderer.lock()->SetTexture(mTextures[mCurrentTextureIndex]);
    }

    mUIRenderer.lock()->SetAlpha(mAlpha);
}

void CMentDisplay::LateUpdate()
{

}

void CMentDisplay::AddTexture(const std::string& Name)
{
	auto texture = RESOURCE.Get<CTexture>(Name);
	if (texture == nullptr)
	{
		return;
	}
	mTextures.push_back(texture);
}

void CMentDisplay::StartDisplay(float DisplayTime, float FadeTime, float WaitTime)
{
    mDisplayTime = DisplayTime;
    mFadeTime = FadeTime;
    mWaitTime = WaitTime;
    mCurrentTextureIndex = 0;
    mTime = 0.0f;
    mAlpha = 0.0f;
    if (!mTextures.empty()) {
        mUIRenderer.lock()->SetTexture(mTextures[mCurrentTextureIndex]);
        owner->SetActive(true);
    } 
}