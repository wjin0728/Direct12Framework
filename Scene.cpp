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
	auto& camera = mCameras["DirectinalLight"];
	if (!camera) {
		return;
	}
	INSTANCE(CDX12Manager).PrepareShadowPass();

	auto shadowPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);

	auto offset = ALIGNED_SIZE(sizeof(CBPassData));
	shadowPassBuffer->BindToShader(offset);

	RenderForLayer("Opaque", camera, SHADOW);

	auto shadowMap = RESOURCE.Get<CTexture>("ShadowMap");
	shadowMap->ChangeResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void CScene::RenderForwardPass()
{
	auto backBuffer = RT_GROUP(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN);
	UINT backBufferIdx = INSTANCE(CDX12Manager).GetCurrBackBufferIdx();

	backBuffer->ChangeResourceToTarget(backBufferIdx);
	backBuffer->SetRenderTarget(backBufferIdx);
	backBuffer->ClearRenderTarget(backBufferIdx);
	backBuffer->ClearDepthStencil();


	auto forwardPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	forwardPassBuffer->BindToShader(0);

	auto& camera = mCameras["MainCamera"];
	if (camera) {
		RenderForLayer("Opaque", camera);

		/*for (const auto& instancingGroup : instancingGroups) {
			instancingGroup->Render(camera);
		}*/
		if(mTerrain) mTerrain->Render(camera);
	}
	auto& uiCamera = mCameras["UICamera"];
	//RenderForLayer("UI", uiCamera);

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
	auto finalBuffer = RT_GROUP(RENDER_TARGET_GROUP_TYPE::FINAL_PASS);
	finalBuffer->ChangeResourceToTarget(0);
	finalBuffer->SetRenderTarget(0);

	//RenderForLayer("UI", false);
}

void CScene::LoadSceneFromFile(const std::string& fileName)
{
	std::ifstream ifs{ fileName, std::ios::binary };
	if (!ifs) {
		return;
	}
	//공유 리소스 로드
	RESOURCE.LoadSceneResourcesFromFile(ifs);
	//프리팹
	std::unordered_map<std::string, std::shared_ptr<CGameObject>> prefabs{};
	CreatePrefabs(ifs, prefabs);

	int rootNum{};
	BinaryReader::ReadDateFromFile(ifs, rootNum);
	for (int i = 0; i < rootNum; i++) {
		auto object = CGameObject::CreateObjectFromFile(ifs, prefabs);
		AddObject("Opaque", object);
	}
}

void CScene::CreatePrefabs(std::ifstream& ifs, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs)
{
	using namespace BinaryReader;
	std::string token{};

	int prefabNum{};
	BinaryReader::ReadDateFromFile(ifs, prefabNum);

	for(int i=0; i< prefabNum; i++) {
		auto prefab = CGameObject::CreateObjectFromFile(ifs, prefabs);
		prefabs[prefab->GetName()] = prefab;
	}
}

std::shared_ptr<CGameObject> CScene::FindObjectWithTag(const std::string& tag)
{
	std::shared_ptr<CGameObject> obj = nullptr;

	for (const auto& [layer, objects] : mRenderLayers) {
		if (obj = FindObjectWithTag(layer, tag))
			return obj;
	}
	return obj;
}

std::shared_ptr<CGameObject> CScene::FindObjectWithTag(const std::string& renderLayer, const std::string& tag)
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

void CScene::AddObject(const std::string& renderLayer, std::shared_ptr<CGameObject> object)
{
	auto itr = findByRawPointer(mObjects, object.get());
	if (itr == mObjects.end()) {
		mObjects.push_back(object);
	}

	if (!mRenderLayers.contains(renderLayer)) {
		mRenderLayers[renderLayer] = ObjectList{};
	}
	auto& objectList = mRenderLayers[renderLayer];

	auto itr2 = findByRawPointer(objectList, object.get());
	if (itr2 == objectList.end()) {
		object->SetRenderLayer(renderLayer);
		objectList.push_back(object);
	}
}

void CScene::AddObject(std::shared_ptr<CGameObject> object)
{
	const std::string& renderLayer = object->GetRenderLayer();

	AddObject(renderLayer, object);
}

void CScene::RemoveObject(std::shared_ptr<CGameObject> object)
{
	auto itr = findByRawPointer(mObjects, object.get());
	if (itr != mObjects.end()) {
		mObjects.erase(itr);
	}

	const std::string& key = object->GetRenderLayer();
	if (mRenderLayers.contains(key)) {
		auto& objectList = mRenderLayers[key];

		auto itr = findByRawPointer(objectList, object.get());
		if (itr != objectList.end()) {
			objectList.erase(itr);
		}
	}
}

void CScene::SetTerrain(std::shared_ptr<CTerrain> terrain)
{
	mTerrain = terrain;
}

void CScene::AddCamera(std::shared_ptr<CCamera> camera)
{
	auto& tag = camera->GetOwner()->GetTag();
	if (mCameras.contains(tag)) {
		return;
	}

	mCameras[tag] = camera;
}

void CScene::RemoveCamera(const std::string& tag)
{
	if (!mCameras.contains(tag)) {
		return;
	}

	mCameras.erase(tag);
}

void CScene::RenderForLayer(const std::string& layer, std::shared_ptr<CCamera> camera, int pass)
{
	if (!mRenderLayers.contains(layer)) {
		return;
	}
	camera->SetViewportsAndScissorRects(CMDLIST);
	for (const auto& object : mRenderLayers[layer]) {
		object->Render(camera, pass);
	}
}

void CScene::RenderTerrain(const std::string& layer)
{
	if (!mTerrain) {
		return;
	}
	if (!mShaders.contains(layer)) {
		return;
	}
	mShaders[layer]->SetPipelineState(CMDLIST);
	mTerrain->Render(mCameras["MainCamera"]);
}

void CScene::UpdatePassData()
{
	CBPassData passData;
	Matrix T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	auto& camera = mCameras["MainCamera"];
	auto& lightCamera = mCameras["DirectinalLight"];


	if (camera) {
		passData.camPos = camera->GetLocalPosition();
		passData.viewProjMat = camera->GetViewProjMat().Transpose();

		if (lightCamera) {
			passData.shadowTransform = (lightCamera->GetViewOrthoProjMat() * T).Transpose();
			passData.shadowMapIdx = RESOURCE.Get<CTexture>("ShadowMap")->GetSrvIndex();
		}
	}
	passData.deltaTime = DELTA_TIME;
	passData.totalTime = TIMER.GetTotalTime();
	passData.renderTargetSize = INSTANCE(CDX12Manager).GetRenderTargetSize();

	passData.gFogRange = 50.f;
	passData.gFogStart = 20.f;
	passData.gFogColor = Color(0.2f, 0.2f, 0.2f);

	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::PASS)->UpdateBuffer(0, &passData, sizeof(CBPassData));

	if (lightCamera) {
		passData.camPos = lightCamera->GetLocalPosition();
		passData.viewProjMat = lightCamera->GetViewOrthoProjMat().Transpose();
	}

	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::PASS)->UpdateBuffer(ALIGNED_SIZE(sizeof(CBPassData)), &passData, sizeof(CBPassData));
}
