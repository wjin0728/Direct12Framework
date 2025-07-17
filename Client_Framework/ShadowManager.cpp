#include "stdafx.h"
#include "ShadowManager.h"
#include "Light.h"
#include "GameObject.h"
#include "Transform.h"
#include "DX12Manager.h"
#include "Scene.h"
#include"ObjectPoolManager.h"
#include"Mesh.h"
#include"ResourceManager.h"
#include"Shader.h"
#include"Camera.h"
#include"SceneManager.h"
#include"Renderer.h"
#include"Timer.h"
#include"FrameResource.h"
#include"RenderManager.h"
#include "DescriptorHeaps.h"
#include"RenderTargetGroup.h"
#include"Texture.h"


void CShadowManager::Initialize(int cascadeLevels, float shadowMapSize)
{
	mCascadeCount = cascadeLevels;
	mShadowMapSize = shadowMapSize;

	for(int i = 0; i < mCascadeCount; ++i)
	{

		mViewports[i].Height = mShadowMapSize;
		mViewports[i].Width = mShadowMapSize;
		mViewports[i].MaxDepth = 1.0f;
		mViewports[i].MinDepth = 0.0f;
		mViewports[i].TopLeftX = mShadowMapSize * i;
		mViewports[i].TopLeftY = 0.0f;

		mScissorRects[i].left = mViewports[i].TopLeftX;
		mScissorRects[i].top = mViewports[i].TopLeftY;
		mScissorRects[i].right = mViewports[i].TopLeftX + mShadowMapSize;
		mScissorRects[i].bottom = mViewports[i].TopLeftY + mShadowMapSize;

	}
	mCascadePartitionMax = 100.f;
	mCascadePartitions[0] = 10.f;
	mCascadePartitions[1] = 30.f;
	mCascadePartitions[2] = 100.f;
	mCascadePartitions[3] = 100.f;


	mShadowDataCB = std::make_shared<CConstantBuffer>();
	mShadowDataCB->Initialize(8, ALIGNED_SIZE(sizeof(CBAllShadowData)));

	auto shadowMap = std::make_shared<CTexture>
		(
			"ShadowMap",
			DXGI_FORMAT_R32_TYPELESS,
			nullptr, 0,
			mShadowMapSize * mCascadeCount, mShadowMapSize,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
	shadowMap->SetTextureType(DEPTH_STENCIL);
	RESOURCE.Add(shadowMap);
	RESOURCE.ProcessGPULoadQueue();

	INSTANCE(CDX12Manager).GetDescriptorHeaps()->CreateDSV(shadowMap, DS_TYPE::SHADOW_MAP);
}

void CShadowManager::Destroyi()
{
	if (mShadowDataCB) 
	{
		mShadowDataCB.reset();
	}
	for (int i = 0; i < mCascadeCount; ++i) 
	{
		mViewports[i] = {};
		mScissorRects[i] = {};
	}
	mSceneOBBWS = BoundingOrientedBox{};
}

void CShadowManager::UpdateSceneBoundingBox(const BoundingBox& boundingBox)
{
	BoundingOrientedBox::CreateFromBoundingBox(mSceneOBBWS, boundingBox);
	if (mLightCamera) {
		Vec3 lightDirection = mLightCamera->GetTransform()->GetWorldLook();
		mLightCamera->GetTransform()->SetLocalPosition(mSceneOBBWS.Center - lightDirection * 10000.f);
		mLightCamera->GenerateViewMatrix();
	}
}

void CShadowManager::CreateFrustumPointsFromCascadeIntervals(float cascadeIntervalStart, float cascadeIntervalEnd, Vec3 frustumPoints[8])
{
	/*BoundingFrustum frustum(projection);
	frustum.Near = cascadeIntervalStart;
	frustum.Far = cascadeIntervalEnd;

	frustum.GetCorners(frustumPoints);*/
}

struct Triangle
{
    XMVECTOR pt[3];
    bool culled;
};


void CShadowManager::CalculateNearFar(Vec3 sceneBoundCornersLS[8], float& nearPlane, float& farPlane, Vec2 vLightCameraOrthographicMin,
	Vec2 vLightCameraOrthographicMax)
{
    // Initialize the near and far planes
    nearPlane = FLT_MAX;
    farPlane = -FLT_MAX;

    Triangle triangleList[16];
    INT iTriangleCnt = 1;

    triangleList[0].pt[0] = sceneBoundCornersLS[0];
    triangleList[0].pt[1] = sceneBoundCornersLS[1];
    triangleList[0].pt[2] = sceneBoundCornersLS[2];
    triangleList[0].culled = false;

    // These are the indices used to tesselate an AABB into a list of triangles.
    static const INT iAABBTriIndexes[] =
    {
        0,1,2,  1,2,3,
        4,5,6,  5,6,7,
        0,2,4,  2,4,6,
        1,3,5,  3,5,7,
        0,1,4,  1,4,5,
        2,3,6,  3,6,7
    };

    INT iPointPassesCollision[3];

    float fLightCameraOrthographicMinX = XMVectorGetX(vLightCameraOrthographicMin);
    float fLightCameraOrthographicMaxX = XMVectorGetX(vLightCameraOrthographicMax);
    float fLightCameraOrthographicMinY = XMVectorGetY(vLightCameraOrthographicMin);
    float fLightCameraOrthographicMaxY = XMVectorGetY(vLightCameraOrthographicMax);

    for (INT AABBTriIter = 0; AABBTriIter < 12; ++AABBTriIter)
    {

        triangleList[0].pt[0] = sceneBoundCornersLS[iAABBTriIndexes[AABBTriIter * 3 + 0]];
        triangleList[0].pt[1] = sceneBoundCornersLS[iAABBTriIndexes[AABBTriIter * 3 + 1]];
        triangleList[0].pt[2] = sceneBoundCornersLS[iAABBTriIndexes[AABBTriIter * 3 + 2]];
        iTriangleCnt = 1;
        triangleList[0].culled = FALSE;

        // Clip each invidual triangle against the 4 frustums.  When ever a triangle is clipped into new triangles, 
        //add them to the list.
        for (INT frustumPlaneIter = 0; frustumPlaneIter < 4; ++frustumPlaneIter)
        {

            FLOAT fEdge;
            INT iComponent;

            if (frustumPlaneIter == 0)
            {
                fEdge = fLightCameraOrthographicMinX; // todo make float temp
                iComponent = 0;
            }
            else if (frustumPlaneIter == 1)
            {
                fEdge = fLightCameraOrthographicMaxX;
                iComponent = 0;
            }
            else if (frustumPlaneIter == 2)
            {
                fEdge = fLightCameraOrthographicMinY;
                iComponent = 1;
            }
            else
            {
                fEdge = fLightCameraOrthographicMaxY;
                iComponent = 1;
            }

            for (INT triIter = 0; triIter < iTriangleCnt; ++triIter)
            {
                // We don't delete triangles, so we skip those that have been culled.
                if (!triangleList[triIter].culled)
                {
                    INT iInsideVertCount = 0;
                    XMVECTOR tempOrder;
                    // Test against the correct frustum plane.
                    // This could be written more compactly, but it would be harder to understand.

                    if (frustumPlaneIter == 0)
                    {
                        for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (XMVectorGetX(triangleList[triIter].pt[triPtIter]) >
                                XMVectorGetX(vLightCameraOrthographicMin))
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else if (frustumPlaneIter == 1)
                    {
                        for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (XMVectorGetX(triangleList[triIter].pt[triPtIter]) <
                                XMVectorGetX(vLightCameraOrthographicMax))
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else if (frustumPlaneIter == 2)
                    {
                        for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (XMVectorGetY(triangleList[triIter].pt[triPtIter]) >
                                XMVectorGetY(vLightCameraOrthographicMin))
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else
                    {
                        for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (XMVectorGetY(triangleList[triIter].pt[triPtIter]) <
                                XMVectorGetY(vLightCameraOrthographicMax))
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }

                    // Move the points that pass the frustum test to the begining of the array.
                    if (iPointPassesCollision[1] && !iPointPassesCollision[0])
                    {
                        tempOrder = triangleList[triIter].pt[0];
                        triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = tempOrder;
                        iPointPassesCollision[0] = TRUE;
                        iPointPassesCollision[1] = FALSE;
                    }
                    if (iPointPassesCollision[2] && !iPointPassesCollision[1])
                    {
                        tempOrder = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = triangleList[triIter].pt[2];
                        triangleList[triIter].pt[2] = tempOrder;
                        iPointPassesCollision[1] = TRUE;
                        iPointPassesCollision[2] = FALSE;
                    }
                    if (iPointPassesCollision[1] && !iPointPassesCollision[0])
                    {
                        tempOrder = triangleList[triIter].pt[0];
                        triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = tempOrder;
                        iPointPassesCollision[0] = TRUE;
                        iPointPassesCollision[1] = FALSE;
                    }

                    if (iInsideVertCount == 0)
                    { // All points failed. We're done,  
                        triangleList[triIter].culled = true;
                    }
                    else if (iInsideVertCount == 1)
                    {// One point passed. Clip the triangle against the Frustum plane
                        triangleList[triIter].culled = false;

                        // 
                        XMVECTOR vVert0ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[0];
                        XMVECTOR vVert0ToVert2 = triangleList[triIter].pt[2] - triangleList[triIter].pt[0];

                        // Find the collision ratio.
                        FLOAT fHitPointTimeRatio = fEdge - XMVectorGetByIndex(triangleList[triIter].pt[0], iComponent);
                        // Calculate the distance along the vector as ratio of the hit ratio to the component.
                        FLOAT fDistanceAlongVector01 = fHitPointTimeRatio / XMVectorGetByIndex(vVert0ToVert1, iComponent);
                        FLOAT fDistanceAlongVector02 = fHitPointTimeRatio / XMVectorGetByIndex(vVert0ToVert2, iComponent);
                        // Add the point plus a percentage of the vector.
                        vVert0ToVert1 *= fDistanceAlongVector01;
                        vVert0ToVert1 += triangleList[triIter].pt[0];
                        vVert0ToVert2 *= fDistanceAlongVector02;
                        vVert0ToVert2 += triangleList[triIter].pt[0];

                        triangleList[triIter].pt[1] = vVert0ToVert2;
                        triangleList[triIter].pt[2] = vVert0ToVert1;

                    }
                    else if (iInsideVertCount == 2)
                    { // 2 in  // tesselate into 2 triangles


                        // Copy the triangle\(if it exists) after the current triangle out of
                        // the way so we can override it with the new triangle we're inserting.
                        triangleList[iTriangleCnt] = triangleList[triIter + 1];

                        triangleList[triIter].culled = false;
                        triangleList[triIter + 1].culled = false;

                        // Get the vector from the outside point into the 2 inside points.
                        XMVECTOR vVert2ToVert0 = triangleList[triIter].pt[0] - triangleList[triIter].pt[2];
                        XMVECTOR vVert2ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[2];

                        // Get the hit point ratio.
                        FLOAT fHitPointTime_2_0 = fEdge - XMVectorGetByIndex(triangleList[triIter].pt[2], iComponent);
                        FLOAT fDistanceAlongVector_2_0 = fHitPointTime_2_0 / XMVectorGetByIndex(vVert2ToVert0, iComponent);
                        // Calcaulte the new vert by adding the percentage of the vector plus point 2.
                        vVert2ToVert0 *= fDistanceAlongVector_2_0;
                        vVert2ToVert0 += triangleList[triIter].pt[2];

                        // Add a new triangle.
                        triangleList[triIter + 1].pt[0] = triangleList[triIter].pt[0];
                        triangleList[triIter + 1].pt[1] = triangleList[triIter].pt[1];
                        triangleList[triIter + 1].pt[2] = vVert2ToVert0;

                        //Get the hit point ratio.
                        FLOAT fHitPointTime_2_1 = fEdge - XMVectorGetByIndex(triangleList[triIter].pt[2], iComponent);
                        FLOAT fDistanceAlongVector_2_1 = fHitPointTime_2_1 / XMVectorGetByIndex(vVert2ToVert1, iComponent);
                        vVert2ToVert1 *= fDistanceAlongVector_2_1;
                        vVert2ToVert1 += triangleList[triIter].pt[2];
                        triangleList[triIter].pt[0] = triangleList[triIter + 1].pt[1];
                        triangleList[triIter].pt[1] = triangleList[triIter + 1].pt[2];
                        triangleList[triIter].pt[2] = vVert2ToVert1;
                        // Cncrement triangle count and skip the triangle we just inserted.
                        ++iTriangleCnt;
                        ++triIter;


                    }
                    else
                    { // all in
                        triangleList[triIter].culled = false;

                    }
                }// end if !culled loop            
            }
        }
        for (INT index = 0; index < iTriangleCnt; ++index)
        {
            if (!triangleList[index].culled)
            {
                // Set the near and far plan and the min and max z values respectivly.
                for (int vertind = 0; vertind < 3; ++vertind)
                {
                    float fTriangleCoordZ = XMVectorGetZ(triangleList[index].pt[vertind]);
                    if (nearPlane > fTriangleCoordZ)
                    {
                        nearPlane = fTriangleCoordZ;
                    }
                    if (farPlane < fTriangleCoordZ)
                    {
                        farPlane = fTriangleCoordZ;
                    }
                }
            }
        }
    }
}



void CShadowManager::Update()
{
	if(!mViewCamera || !mLightCamera) return;

	const Matrix& viewCamProjection = mViewCamera->GetPerspectiveProjectMat();
	const Matrix& viewCamView = mViewCamera->GetViewMat();
	const Matrix& lightCamView = mLightCamera->GetViewMat();

	float nearFarDistance = mViewCamera->GetFar() - mViewCamera->GetNear();

	BoundingOrientedBox mSceneOBBLS{};
	mSceneOBBWS.Transform(mSceneOBBLS, lightCamView);

	std::array<Vec3, 8> sceneCornersLS{};
	mSceneOBBLS.GetCorners(sceneCornersLS.data());

	Vec3 sceneAABBMinLS = Vec3::FLTMax;
	Vec3 sceneAABBMaxLS = Vec3::FLTMin;
	for (const auto& corner : sceneCornersLS) {
		sceneAABBMinLS = Vec3::Min(sceneAABBMinLS, corner);
		sceneAABBMaxLS = Vec3::Max(sceneAABBMaxLS, corner);
	}

	float nearPlane = sceneAABBMinLS.z;
	float farPlane = sceneAABBMaxLS.z;

	mFitToScene = false; 
	for (int i = 0; i < mCascadeCount;i++) 
	{
		float cascadeIntervalStart = 0.0f;
		float cascadeIntervalEnd = 0.0f;
		if (mFitToScene || i == 0) {
			cascadeIntervalStart = 0.f;
		}
		else {
			cascadeIntervalStart = (float)mCascadePartitions[i - 1] / mCascadePartitionMax * nearFarDistance;
		}
		cascadeIntervalEnd = (float)mCascadePartitions[i] / mCascadePartitionMax * nearFarDistance;

		BoundingFrustum viewCamFrustum = mViewCamera->mFrustumWorld;
		viewCamFrustum.Near = cascadeIntervalStart;
		viewCamFrustum.Far = cascadeIntervalEnd;

		std::array<Vec3, 8> viewFrustumCornersWS{};
		viewCamFrustum.GetCorners(viewFrustumCornersWS.data());

		std::array<Vec3, 8> viewFrustumCornersLS{};
		for (size_t i = 0; i < viewFrustumCornersWS.size(); ++i) {
			viewFrustumCornersLS[i] = Vec3::Transform(viewFrustumCornersWS[i], lightCamView);
		}

		Vec3 viewFrustumCornersLSMin = Vec3::FLTMax;
		Vec3 viewFrustumCornersLSMax = Vec3::FLTMin;
		for (const auto& corner : viewFrustumCornersLS) {
			viewFrustumCornersLSMin = Vec3::Min(viewFrustumCornersLSMin, corner);
			viewFrustumCornersLSMax = Vec3::Max(viewFrustumCornersLSMax, corner);
		}
		viewFrustumCornersLSMin = Vec3::Max(viewFrustumCornersLSMin, sceneAABBMinLS);
		viewFrustumCornersLSMax = Vec3::Min(viewFrustumCornersLSMax, sceneAABBMaxLS);

		float minZ = viewFrustumCornersLSMin.z;
		Vec2 worldUnitsPerTexel{};

		Vec2 lightProjectionMin = viewFrustumCornersLSMin;
		Vec2 lightProjectionMax = viewFrustumCornersLSMax;
		if (mFitToScene)
		{
			Vec3 maxDiagonal = viewFrustumCornersWS[0] - viewFrustumCornersWS[6];
			float projectionMaxSize = maxDiagonal.Length();
			Vec2 bound = Vec2(projectionMaxSize, projectionMaxSize);

			Vec2 lightProjectionSize = lightProjectionMax - lightProjectionMin;
			Vec2 offset = (bound - lightProjectionSize) * 0.5f;

			lightProjectionMin -= offset;
			lightProjectionMax += offset;

			worldUnitsPerTexel = Vec2(projectionMaxSize / mShadowMapSize);
		}
		else
		{
			float scaleAmount = (3 * 2 + 1) / (float)mShadowMapSize;
			Vec2 lightProjectionSize = lightProjectionMax - lightProjectionMin;
			Vec2 offset = lightProjectionSize * 0.5f * scaleAmount;

			lightProjectionMin -= offset;
			lightProjectionMax += offset;

			worldUnitsPerTexel = (lightProjectionMax - lightProjectionMin) / mShadowMapSize;
		}

		lightProjectionMin /= worldUnitsPerTexel;
		lightProjectionMin.Floor();
		lightProjectionMin *= worldUnitsPerTexel;
		lightProjectionMax /= worldUnitsPerTexel;
		lightProjectionMax.Floor();
		lightProjectionMax *= worldUnitsPerTexel;

		CalculateNearFar(sceneCornersLS.data(), nearPlane, farPlane, lightProjectionMin, lightProjectionMax);

		mCascadeProjection[i] = Matrix::CreateOrthographicOffCenter(lightProjectionMin.x, lightProjectionMax.x, 
			lightProjectionMin.y, lightProjectionMax.y, nearPlane, farPlane);

		mCascadeIntervalEnd[i] = cascadeIntervalEnd;
	}
	mView[0] = lightCamView;

	CBAllShadowData* data = reinterpret_cast<CBAllShadowData*>(mShadowDataCB->mappedData);
	data->lightDirection = mLightCamera->GetTransform()->GetWorldLook();
	data->shadowMapIdx = mShadowMapTextureIdx;
	data->shadowViewMat = lightCamView.Transpose();
	data->cascadeCount = mCascadeCount;
	memcpy(data->cascadeFrustumEyeSpaceDepth, mCascadeIntervalEnd, sizeof(float) * mCascadeCount);
	data->cascadeBlend = 0.05f;
	data->shadowMapSize = mShadowMapSize;
	data->texelSize = 1.0f / mShadowMapSize;
	data->nativeTexelSize = data->texelSize / mCascadeCount;
	data->shadowPartition = static_cast<float>(1) / mCascadeCount;
	data->maxBorder = (mShadowMapSize - 1.0f) / mShadowMapSize;
	data->minBorder = 1.0f / mShadowMapSize;

	Matrix T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	);

	auto passDataBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	for (int i = 0; i < mCascadeCount; ++i) 
	{
		CBPassData* cascadeViewProjection = reinterpret_cast<CBPassData*>(passDataBuffer->mappedData + ALIGNED_SIZE(sizeof(CBPassData)) * (i + 1));
		cascadeViewProjection->viewProjMat = (lightCamView * mCascadeProjection[i]).Transpose();

		Matrix cascadeShadowtextureTransform = mCascadeProjection[i] * T;
		data->cascadeScale[i] = Vec4(cascadeShadowtextureTransform._11, cascadeShadowtextureTransform._22, cascadeShadowtextureTransform._33, 1.0f);
		data->cascadeOffset[i] = Vec4(cascadeShadowtextureTransform._41, cascadeShadowtextureTransform._42, cascadeShadowtextureTransform._43, 0.0f);
	}
}

void CShadowManager::RenderShadowMaps()
{
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	if (!scene) return;
	if (!mViewCamera || !mLightCamera) return;

	for(int i = 0; i < mCascadeCount; ++i) 
	{
		auto passDataBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
		passDataBuffer->BindToShader(ALIGNED_SIZE(sizeof(CBPassData)) * (i + 1));
		CMDLIST->RSSetViewports(1, &mViewports[i]);
		CMDLIST->RSSetScissorRects(1, &mScissorRects[i]);
        INSTANCE(CRenderManager).RenderLayer(SHADOW, RENDER_LAYER::Opaque, mViewCamera);
		INSTANCE(CRenderManager).RenderInstancingGroup(SHADOW);
	}
}

void CShadowManager::BindShadowData()
{
	mShadowDataCB->BindToShader(0);
}
