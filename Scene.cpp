#include "stdafx.h"
#include"GameObject.h"
#include "Scene.h"
#include"SceneManager.h"
#include"Shader.h"
#include"Terrain.h"
#include"LightManager.h"
#include"DX12Manager.h"
#include"ResourceManager.h"
#include"Timer.h"
#include"Camera.h"
#include"InstancingGroup.h"


CScene::CScene()
{

}

void CScene::Awake()
{
	for (const auto& object : mObjects) {
		object->Awake();
	}
}

void CScene::Start()
{
	for (const auto& object : mObjects) {
		object->Start();
	}
}

void CScene::Update()
{
	for (const auto& object : mObjects) {
		object->Update();
	}
}

void CScene::LateUpdate()
{
	for (const auto& object : mObjects) {
		object->LateUpdate();
	}
	INSTANCE(CResourceManager).UpdateMaterials();
	UpdatePassData();
}

void CScene::RenderShadowPass()
{
	auto& camera = mCameras[L"DirectinalLight"];
	if (!camera) {
		return;
	}
	if (!mShaders.contains(L"Shadow") || !mRenderLayers.contains(L"Opaque")) {
		return;
	}

	INSTANCE(CDX12Manager).PrepareShadowPass();

	auto shadowPassBuffer = UPLOADBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	shadowPassBuffer->UpdateBuffer(1);

	mShaders[L"Shadow"]->SetPipelineState(CMDLIST);
	camera->SetViewportsAndScissorRects(CMDLIST);

	auto& objects = mRenderLayers[L"Opaque"];
	for (const auto& object : objects) {
		if (!camera->IsInFrustum(object)) continue;
		object->Render();
	}

	auto shadowMap = RESOURCE.Get<CTexture>(L"ShadowMap");
	 
	shadowMap->ChangeResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void CScene::RenderForwardPass()
{
	auto& backBuffer = RT_GROUP(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN);
	UINT backBufferIdx = INSTANCE(CDX12Manager).GetCurrBackBufferIdx();

	backBuffer->ChangeResourceToTarget(backBufferIdx);
	backBuffer->SetRenderTarget(backBufferIdx);
	backBuffer->ClearRenderTarget(backBufferIdx);
	backBuffer->ClearDepthStencil();

	auto& camera = mCameras[L"MainCamera"];
	if (!camera) {
		return;
	}

	auto forwardPassBuffer = UPLOADBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	forwardPassBuffer->UpdateBuffer(0);

	camera->SetViewportsAndScissorRects(CMDLIST);

	RenderForLayer(L"SkyBox", false);
	RenderForLayer(L"Opaque");
	RenderTerrain(L"Terrain");

	for (const auto& instancingGroup : instancingGroups) {
		instancingGroup->Render(camera);
	}

	RenderForLayer(L"UI", false);

	backBuffer->ChangeTargetToResource(backBufferIdx);
}

void CScene::RenderGBufferPass()
{
}

void CScene::RenderLightingPass()
{
}

void CScene::RenderFinalPass()
{
	auto& finalBuffer = RT_GROUP(RENDER_TARGET_GROUP_TYPE::FINAL_PASS);
	finalBuffer->ChangeResourceToTarget(0);
	finalBuffer->SetRenderTarget(0);

	RenderForLayer(L"UI", false);
}

std::shared_ptr<CGameObject> CScene::FindObjectWithTag(const std::wstring& tag)
{
	std::shared_ptr<CGameObject> obj = nullptr;

	for (const auto& [layer, objects] : mRenderLayers) {
		if (obj = FindObjectWithTag(layer, tag))
			return obj;
	}
	return obj;
}

std::shared_ptr<CGameObject> CScene::FindObjectWithTag(const std::wstring& renderLayer, const std::wstring& tag)
{
	for (const auto& object : mRenderLayers[renderLayer]) {
		if (object->GetTag() == tag) {
			return object;
		}
		auto& children = object->GetChildren();
		for (const auto& child : children) {
			if (child->GetTag() == tag) {
				return child;
			}
		}
	}
	return nullptr;
}

void CScene::AddObject(const std::wstring& renderLayer, std::shared_ptr<CGameObject> object)
{
	auto itr = findByRawPointer(mObjects, object.get());
	if (itr == mObjects.end()) {
		mObjects.push_back(object);
	}

	if (mRenderLayers.contains(renderLayer)) {
		auto& objectList = mRenderLayers[renderLayer];

		auto itr = findByRawPointer(objectList, object.get());
		if (itr == objectList.end()) {
			object->SetRenderLayer(renderLayer);
			objectList.push_back(object);
		}
	}
}

void CScene::AddObject(std::shared_ptr<CGameObject> object)
{
	const std::wstring& renderLayer = object->GetRenderLayer();

	AddObject(renderLayer, object);
}

void CScene::RemoveObject(std::shared_ptr<CGameObject> object)
{
	auto itr = findByRawPointer(mObjects, object.get());
	if (itr != mObjects.end()) {
		mObjects.erase(itr);
	}

	const std::wstring& key = object->GetRenderLayer();
	if (mRenderLayers.contains(key)) {
		auto& objectList = mRenderLayers[key];

		auto itr = findByRawPointer(objectList, object.get());
		if (itr != objectList.end()) {
			objectList.erase(itr);
		}
	}
}

void CScene::AddCamera(std::shared_ptr<CCamera> camera)
{
	auto& tag = camera->GetOwner()->GetTag();
	if (mCameras.contains(tag)) {
		return;
	}

	mCameras[tag] = camera;
}

void CScene::RemoveCamera(const std::wstring& tag)
{
	if (!mCameras.contains(tag)) {
		return;
	}

	mCameras.erase(tag);
}

void CScene::RenderForLayer(const std::wstring& layer, bool frustumCulling)
{
	if (!mRenderLayers.contains(layer)) {
		return;
	}

	mShaders[layer]->SetPipelineState(CMDLIST);
	if(frustumCulling) {
		auto& camera = mCameras[L"MainCamera"];
		for (const auto& object : mRenderLayers[layer]) {
			if (!camera->IsInFrustum(object)) continue;
			object->Render();
		}
	}
	else {
		for (const auto& object : mRenderLayers[layer]) {
			object->Render();
		}
	}
}

void CScene::RenderTerrain(const std::wstring& layer)
{
	if (!mTerrain) {
		return;
	}
	if (!mShaders.contains(layer)) {
		return;
	}
	mShaders[layer]->SetPipelineState(CMDLIST);
	mTerrain->Render(mCameras[L"MainCamera"]);
}

void CScene::UpdatePassData()
{
	CBPassData passData;
	Matrix T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	auto& camera = mCameras[L"MainCamera"];
	auto& lightCamera = mCameras[L"DirectinalLight"];


	if (camera) {
		passData.camPos = camera->GetLocalPosition();
		passData.viewProjMat = camera->GetViewProjMat().Transpose();

		if (lightCamera) {
			passData.shadowTransform = (lightCamera->GetViewOrthoProjMat() * T).Transpose();
			passData.shadowMapIdx = RESOURCE.Get<CTexture>(L"ShadowMap")->GetSrvIndex();
		}
	}
	passData.deltaTime = DELTA_TIME;
	passData.totalTime = TIMER.GetTotalTime();
	passData.renderTargetSize = INSTANCE(CDX12Manager).GetRenderTargetSize();

	passData.gFogRange = 120.f;
	passData.gFogStart = 100.f;
	passData.gFogColor = Color(0.6f, 0.6f, 0.6f);

	if (mTerrain) {
		auto terrainMat = mTerrain->GetMaterial();

		passData.terrainMat.material.albedoColor = terrainMat->mAlbedoColor;
		passData.terrainMat.material.emissiveColor = terrainMat->mEmissiveColor;
		passData.terrainMat.material.fresnelR0 = terrainMat->mFresnelR0;
		passData.terrainMat.material.specularColor = terrainMat->mSpecularColor;
		passData.terrainMat.material.diffuseMapIdx = terrainMat->mDiffuseMapIdx;
		passData.terrainMat.material.normalMapIdx = terrainMat->mNormalMapIdx;
		passData.terrainMat.detailMapIdx = terrainMat->mDetailMap1Idx;
		passData.terrainMat.heightMapIdx = RESOURCE.Get<CTexture>(L"heightMap")->GetSrvIndex();
		passData.terrainMat.scale = mTerrain->GetScale();
	}
	UPLOADBUFFER(CONSTANT_BUFFER_TYPE::PASS)->CopyData(&passData);

	if (lightCamera) {
		passData.camPos = lightCamera->GetLocalPosition();
		passData.viewProjMat = lightCamera->GetViewOrthoProjMat().Transpose();
	}

	UPLOADBUFFER(CONSTANT_BUFFER_TYPE::PASS)->CopyData(&passData, 1);
}
