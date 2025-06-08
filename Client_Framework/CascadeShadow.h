#pragma once
#include"stdafx.h"
#include "Component.h"

class CCascadeShadow :
    public CComponent
{
private:
    std::shared_ptr<class CCamera> mCamera{};
	D3D12_VIEWPORT mViewports[CASCADE_COUNT]{};

    float mShadowMapSize = 1024.0f;
    int mCascadeCount = CASCADE_COUNT;
    float mShadowPartition = 0.1f;
    float mCascadeBlend = 0.1f;
    float mTexelSize = 1.0f / mShadowMapSize;
    float mNativeTexelSize = 1.0f / mShadowMapSize;

    Vec4 mCascadeOffset[CASCADE_COUNT];
    Vec4 mCascadeScale[CASCADE_COUNT];
    Vec4 mCascadeFrustumEyeSpaceDepth[CASCADE_COUNT];

	Vec3 mSceneMin = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3 mSceneMax = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

public:
    CCascadeShadow() {};
    ~CCascadeShadow() override {};

    void SetCamera(std::shared_ptr<class CCamera> camera) { mCamera = camera; }
    std::shared_ptr<class CCamera> GetCamera() const { return mCamera; }

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
    virtual void LateUpdate() override;

public:

};

