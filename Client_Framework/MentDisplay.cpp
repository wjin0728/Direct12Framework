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
	mCurrentTextureIndex = 0;

	mUIRenderer.lock()->SetTexture(mTextures[mCurrentTextureIndex]);
}

void CMentDisplay::Update()
{
	float deltaTime = DELTA_TIME;
	//fade in
	if (mAlpha < 1.0f)
	{
		mAlpha += 0.01f;
		if (mAlpha > 1.0f)
		{
			mAlpha = 1.0f;
		}
		mUIRenderer.lock()->SetAlpha(mAlpha);
	}

	if(mAlpha >= 1.0f)
	{
		mTime += deltaTime;
		if (mTime >= 2.0f) // Change texture every 2 seconds
		{
			mCurrentTextureIndex++;
			if (mCurrentTextureIndex >= mTextures.size())
			{
				mCurrentTextureIndex = 0; // Loop back to the first texture
			}
			mUIRenderer.lock()->SetTexture(mTextures[mCurrentTextureIndex]);
			mTime = 0.0f; // Reset time
		}
	}
	else
	{
		mUIRenderer.lock()->SetAlpha(mAlpha);
	}
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
