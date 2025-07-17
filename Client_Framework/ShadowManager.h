#pragma once
#include"stdafx.h"

class CShadowManager
{
	MAKE_SINGLETON(CShadowManager)

private:
    D3D12_VIEWPORT mViewports[CASCADE_COUNT]{};
    D3D12_RECT mScissorRects[CASCADE_COUNT]{};

    Matrix mProjection[CASCADE_COUNT]{};
	Matrix mView[CASCADE_COUNT]{};

    float mShadowMapSize = 1024.0f;
    int mCascadeCount = CASCADE_COUNT;
    float mCascadePartitionMax = 0.1f;
    float mCascadeBlend = 0.1f;
    float mTexelSize = 1.0f / mShadowMapSize;
    float mNativeTexelSize = 1.0f / mShadowMapSize;

	int mShadowMapTextureIdx = -1;

    Vec4 mCascadeOffset[CASCADE_COUNT];
    Vec4 mCascadeScale[CASCADE_COUNT];
    int mCascadePartitions[CASCADE_COUNT];
	float mCascadeIntervalEnd[CASCADE_COUNT];
	Matrix mCascadeProjection[CASCADE_COUNT];

    std::shared_ptr<class CConstantBuffer> mShadowDataCB{};

    BoundingOrientedBox mSceneOBBWS{};

	int mPCFEnabled = 0;
	int mPCFBlurRadius = 1;

	bool mFitToScene = true;

public:
    class CCamera* mViewCamera{};
    class CCamera* mLightCamera{};

	void Initialize(int cascadeLevels, float shadowMapSize);
	void Destroyi();
    void Update();

	void SetShadowMapTextureIdx(int idx) { mShadowMapTextureIdx = idx; }
	void SetSceneOBBWS(const BoundingOrientedBox& obb) { mSceneOBBWS = obb; }


	void UpdateSceneBoundingBox(const BoundingBox& boundingBox);


    void CreateFrustumPointsFromCascadeIntervals(float cascadeIntervalStart, float cascadeIntervalEnd, Vec3 frustumPoints[8]);
    void CalculateNearFar(Vec3 sceneBoundCornersLS[8], float& nearPlane, float& farPlane, Vec2 vLightCameraOrthographicMin,
        Vec2 vLightCameraOrthographicMax);

	float GetShadowMapSize() const { return mShadowMapSize; }
    int GetCascadeCount() const { return mCascadeCount; }

	void RenderShadowMaps();
    void BindShadowData();
};

