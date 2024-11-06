#include "stdafx.h"
#include"GameObject.h"
#include "Scene.h"
#include"SceneManager.h"
#include"Player.h"
#include"Shader.h"
#include"Terrain.h"
#include"LightManager.h"
#include"DX12Manager.h"
#include"ResourceManager.h"
#include"Timer.h"
#include"Camera.h"


CScene::CScene()
{

}

void CScene::Awake()
{
	for (const auto& [layer, objects] : mObjects) {
		for (const auto& object : objects) {
			object->Awake();
		}
	}
}

void CScene::Start()
{
	for (const auto& [layer, objects] : mObjects) {
		for (const auto& object : objects) {
			object->Start();
		}
	}
}

void CScene::Update()
{
	for (const auto& [layer, objects] : mObjects) {
		for (const auto& object : objects) {
			object->Update();
		}
	}
}

void CScene::LateUpdate()
{
	for (const auto& [layer, objects] : mObjects) {
		for (const auto& object : objects) {
			object->LateUpdate();
		}
	}
	INSTANCE(CResourceManager).UpdateMaterials();
	UpdatePassData();
}

std::shared_ptr<CGameObject> CScene::FindObjectWithTag(const std::wstring& tag)
{
	std::shared_ptr<CGameObject> obj = nullptr;

	for (const auto& [layer, objects] : mObjects) {
		if (obj = FindObjectWithTag(layer, tag))
			return obj;
	}
	return obj;
}

std::shared_ptr<CGameObject> CScene::FindObjectWithTag(const std::wstring& renderLayer, const std::wstring& tag)
{
	for (const auto& object : mObjects[renderLayer]) {
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
	if (!mObjects.contains(renderLayer)) {
		return;
	}

	auto& objectList = mObjects[renderLayer];

	auto itr = std::find(objectList.begin(), objectList.end(), object);
	if (itr != objectList.end()) {
		return;
	}

	object->SetRenderLayer(renderLayer);
	objectList.push_back(object);
}

void CScene::DestroyObject(std::shared_ptr<CGameObject> object)
{
	const std::wstring& key = object->GetRenderLayer();
	if (!mObjects.contains(key)) {
		return;
	}

	auto& objectList = mObjects[key];

	auto itr = std::find(objectList.begin(), objectList.end(), object);
	if (itr != objectList.end()) {
		objectList.erase(itr);
	}
}

bool CScene::PrepareRender()
{
	const auto& camera = CCamera::GetMainCamera();
	if (!camera) {
		return false;
	}

	camera->SetViewportsAndScissorRects(CMDLIST);

	return true;
}

void CScene::RenderForLayer(const std::wstring& layer, bool frustumCulling)
{
	if (!mObjects.contains(layer)) {
		return;
	}

	mShaders[layer]->SetPipelineState(CMDLIST);
	if(frustumCulling) {
		for (const auto& object : mObjects[layer]) {
			object->Render(CCamera::GetMainCamera());
		}
	}
	else {
		for (const auto& object : mObjects[layer]) {
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
	mTerrain->Render(CCamera::GetMainCamera());
}

void CScene::UpdatePassData()
{
	CBPassData passData;

	const auto& camera = CCamera::GetMainCamera();
	if (camera) {
		passData.camPos = camera->GetLocalPosition();
		passData.projMat = camera->GetPerspectiveProjectMat().Transpose();
		passData.viewMat = camera->GetViewMat().Transpose();
		passData.viewProjMat = camera->GetViewProjMat().Transpose();
	}
	passData.deltaTime = DELTA_TIME;
	passData.totalTime = TIMER.GetTotalTime();
	passData.renderTargetSize = INSTANCE(CDX12Manager).GetRenderTargetSize();

	if (mTerrain) {
		auto terrainMat = mTerrain->GetMaterial();

		passData.terrainMat.material.albedoColor = terrainMat->mAlbedoColor;
		passData.terrainMat.material.emissiveColor = terrainMat->mEmissiveColor;
		passData.terrainMat.material.fresnelR0 = terrainMat->mFresnelR0;
		passData.terrainMat.material.specularColor = terrainMat->mSpecularColor;
		passData.terrainMat.material.diffuseMapIdx = terrainMat->mDiffuseMapIdx;
		passData.terrainMat.material.normalMapIdx = terrainMat->mNormalMapIdx;
		passData.terrainMat.detailMapIdx = terrainMat->mDetailMap1Idx;
	}

	UPLOADBUFFER(CONSTANT_BUFFER_TYPE::PASS)->CopyData(&passData);
}
