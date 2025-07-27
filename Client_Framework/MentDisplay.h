#pragma once
#include"stdafx.h"
#include "Component.h"


class CMentDisplay :
    public CComponent
{
public:
    std::vector<std::shared_ptr<class CTexture>> mTextures;
	std::weak_ptr<class CUIRenderer> mUIRenderer;

	float mAlpha = 1.0f;
	float mDisplayTime = 0.0f;
	float mTime = 0.0f;
	float mFadeTime = 0.0f;
	float mWaitTime = 0.0f;
	int mCurrentTextureIndex = 0;

public:
    CMentDisplay() {}
    virtual ~CMentDisplay() {}

    virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CMentDisplay>(*this); }

public:
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
	virtual void LateUpdate() override;

    void ClearTextures()
    {
        mTextures.clear();
        mCurrentTextureIndex = 0;
        mAlpha = 0.0f;
	}
    void AddTexture(const std::string& Name);
    void StartDisplay(float DisplayTime, float FadeTime, float WaitTime = 0.0f)
    {
        mDisplayTime = DisplayTime;
        mFadeTime = FadeTime;
        mWaitTime = WaitTime;
        mCurrentTextureIndex = 0;
        mTime = 0.0f;
        mAlpha = 0.0f;
	}
    void ClearTextures()
    {
        mTextures.clear();
        mCurrentTextureIndex = 0;
        mAlpha = 0.0f;
        mTime = 0.0f;
	}
};

