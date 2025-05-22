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

	if (!mainCamera || !lightCam) return;

	Vec3 corners[8]{};
	mainCamera->mFrustumView.GetCorners(corners);


	Vec3 center{};
	for (int i = 0; i < 8; ++i) {
		center += corners[i];
	}
	center /= 8.f;
	float max = -FLT_MAX;
	for (int i = 0; i < 8; ++i) {
		if (max < Vec3::Distance(center, corners[i])) max = Vec3::Distance(center, corners[i]);
	}
	float r = max;
	float size = r * 1.5f;
	lightCam->GenerateOrthographicProjectionMatrix(1.f, 100.f, size, size);
}

void CLight::Update()
{
}

void CLight::LateUpdate()
{
	if (!mLightData.type == (UINT)LIGHT_TYPE::DIRECTIONAL || !mainCamera || !lightCam) return;
	Vec3 corners[8]{};
	mainCamera->GenerateViewMatrix();
	mainCamera->mFrustumWorld.GetCorners(corners);

	Vec3 center{};
	for (int i = 0; i < 8; ++i) {
		center += corners[i];
	}
	center /= 8.f;
	float max = -FLT_MAX;
	for (int i = 0; i < 8; ++i) {
		if (max < Vec3::Distance(center, corners[i])) max = Vec3::Distance(center, corners[i]);
	}
	float r = max/2.f;

	auto transform = GetTransform();
	Vec3 dir = transform->GetWorldLook().GetNormalized();
	transform->SetLocalPosition(center - (r * dir));
	transform->LookTo(dir);

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