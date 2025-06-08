#include "stdafx.h"
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

std::array< std::shared_ptr<CMesh>, 3> CLight::volumes{};

CLight::~CLight()
{
	if (mLightIndex < 0) {
		return;
	}
	INSTANCE(CObjectPoolManager).ReturnLightCBVIndex(mLightIndex);
	mLightIndex = -1;
	mLightData = {};

	if (mCbvIdx < 0) {
		return;
	}
	INSTANCE(CObjectPoolManager).ReturnCBVIndex(mCbvIdx);
	mCbvIdx = -1;
	mCbvOffset = 0;
}

std::shared_ptr<CComponent> CLight::Clone()
{
	std::shared_ptr<CLight> copy = std::make_shared<CLight>();
	copy->mLightData = mLightData;
	return copy;
}

void CLight::Awake()
{
}

void CLight::Start()
{
	if (mLightIndex < 0) {
		mLightIndex = INSTANCE(CObjectPoolManager).GetLightCBVIndex();
	}
	if (mCbvIdx < 0) {
		mCbvIdx = INSTANCE(CObjectPoolManager).GetTopCBVIndex();
		mCbvOffset = ALIGNED_SIZE(sizeof(CBObjectData)) * mCbvIdx;

		auto objectBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::OBJECT);
		CBObjectData objDate;
		objDate.worldMAt = GetTransform()->GetWorldMat().Transpose();
		objDate.invWorldMAt = GetTransform()->GetWorldMat().Invert();
		objDate.idx0 = mLightIndex;
		objectBuffer->UpdateBuffer(mCbvOffset, &objDate);
	}
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	mainCamera = scene->GetCamera("MainCamera");

	
	UpdateLightViewBound();
}

void CLight::Update()
{
}

void CLight::LateUpdate()
{
	if (!mLightData.type == (UINT)LIGHT_TYPE::DIRECTIONAL || !mainCamera || !lightCam) return;

	//UpdateLightViewBound();

	auto transform = GetTransform();
	mLightData.position = transform->GetWorldPosition();
	mLightData.direction = transform->GetWorldLook();
}

void CLight::Render(std::shared_ptr<CRenderTargetGroup> renderTarget)
{
	if (mLightIndex < 0) return;
	if (!volumes[mLightData.type]) return;
	auto objectBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::OBJECT);
	objectBuffer->BindToShader(mCbvOffset);
	BindLightDataToShader();

	if (mLightData.type == (UINT)LIGHT_TYPE::POINT || mLightData.type == (UINT)LIGHT_TYPE::SPOT) {
		RESOURCE.Get<CShader>("LightingStencil")->SetPipelineState(CMDLIST);
		renderTarget->SetOnlyDepthStencil();
		renderTarget->ClearOnlyStencil(0);
		volumes[mLightData.type]->Render(CMDLIST, 0);

		RESOURCE.Get<CShader>("LightingLighting")->SetPipelineState(CMDLIST);
		renderTarget->SetRenderTargets();
		volumes[mLightData.type]->Render(CMDLIST, 0);
	}
	else if (mLightData.type == (UINT)LIGHT_TYPE::DIRECTIONAL) {
		RESOURCE.Get<CShader>("LightingDirectional")->SetPipelineState(CMDLIST);
		CRenderer::RenderFullscreen();
	}
}

void CLight::BindLightDataToShader()
{
	auto lightBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::LIGHT);
	UINT offset = mLightIndex * sizeof(CBLightsData);
	lightBuffer->UpdateBuffer(offset, &mLightData);
}

void CLight::SetVolumes()
{
	volumes[(UINT)LIGHT_TYPE::DIRECTIONAL] = RESOURCE.Get<CMesh>("Rectangle");
	volumes[(UINT)LIGHT_TYPE::POINT] = RESOURCE.Get<CMesh>("Sphere");
	//volumes[(UINT)LIGHT_TYPE::SPOT] = CMesh::CreateConeMesh(1.f, 1.f, 10, 10);
}

void CLight::UpdateLightCamData()
{
	if (!mainCamera || !lightCam) return;
	UpdateLightViewBound();

	Vec3 lightFrustumCenter = mFrustumBoundWS.Center;
	
}

void CLight::UpdateLightViewBound()
{
	Vec3 corners[8]{};
	mainCamera->mFrustumWorld.GetCorners(corners);

	BoundingOrientedBox::CreateFromPoints(mFrustumBoundWS, 8, corners, sizeof(Vec3));
	Vec3 lightFrustumCenter = mFrustumBoundWS.Center;
	float lightFrustumRadius = 1000.f; // Arbitrary large radius for directional light

	auto transform = GetTransform();
	Vec3 dir = transform->GetWorldLook();
	transform->SetLocalPosition(lightFrustumCenter - (lightFrustumRadius * dir));
	lightCam->GenerateViewMatrix();

	BoundingOrientedBox mFrustumBoundLS{};
	mFrustumBoundWS.Transform(mFrustumBoundLS, lightCam->GetViewMat());

	std::array<Vec3, 8> frustumCornersLS{};
	mFrustumBoundLS.GetCorners(frustumCornersLS.data());

	auto minmaxX = std::minmax_element(frustumCornersLS.begin(), frustumCornersLS.end(), [](const Vec3& a, const Vec3& b) { return a.x < b.x; });
	auto minmaxY = std::minmax_element(frustumCornersLS.begin(), frustumCornersLS.end(), [](const Vec3& a, const Vec3& b) { return a.y < b.y; });
	auto minmaxZ = std::minmax_element(frustumCornersLS.begin(), frustumCornersLS.end(), [](const Vec3& a, const Vec3& b) { return a.z < b.z; });

	float minX = minmaxX.first->x;
	float maxX = minmaxX.second->x;
	float minY = minmaxY.first->y;
	float maxY = minmaxY.second->y;
	float minZ = minmaxZ.first->z;
	float maxZ = minmaxZ.second->z;
	float sizeX = maxX - minX;
	float sizeY = maxY - minY;

	float currentTime = TIMER.GetTotalTime();
	if (currentTime / 1 == 0) {
		std::cout << "near : " << minZ << " far : " << maxZ << std::endl;
		std::cout << "sizeX : " << sizeX << " sizeY : " << sizeY << std::endl;
	}
	lightCam->GenerateOrthographicProjectionMatrix(minZ, maxZ, 300, 300);
}
