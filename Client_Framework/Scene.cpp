#include "stdafx.h"
#include"GameObject.h"
#include "Scene.h"
#include"SceneManager.h"
#include"Shader.h"
#include"Terrain.h"
#include"DX12Manager.h"
#include"ResourceManager.h"
#include"Timer.h"
#include"Camera.h"
#include"InstancingGroup.h"
#include"Renderer.h"
#include"Light.h"
#include"Transform.h"
#include"PlayerController.h"
#include"RigidBody.h"
#include"ThirdPersonCamera.h"	


CScene::CScene()
{
	auto gbufferAlbedo = RESOURCE.Get<CTexture>("GBufferAlbedo");
	auto gbufferNormal = RESOURCE.Get<CTexture>("GBufferNormal");
	auto gbufferDepth = RESOURCE.Get<CTexture>("GBufferDepth");
	auto gbufferEmissive = RESOURCE.Get<CTexture>("GBufferEmissive");
	auto gbufferPos = RESOURCE.Get<CTexture>("GBufferPosition");

	if (gbufferAlbedo) {
		renderTargetIndices.push_back(gbufferAlbedo->GetSrvIndex());
		renderTargetIndices.push_back(gbufferNormal->GetSrvIndex());
		renderTargetIndices.push_back(gbufferEmissive->GetSrvIndex());
		renderTargetIndices.push_back(gbufferPos->GetSrvIndex());
		renderTargetIndices.push_back(gbufferDepth->GetSrvIndex());
	}
	auto lightingTarget = RESOURCE.Get<CTexture>("LightingTarget");
	if (lightingTarget) {
		int lightingTargetIdx = lightingTarget->GetSrvIndex();
		renderTargetIndices.push_back(lightingTargetIdx);
		renderPasstype = renderTargetIndices.size() - 1;
	}
	auto postProcessTarget = RESOURCE.Get<CTexture>("PostProcessTarget");
	if (postProcessTarget) {
		int postProcessTargetIdx = postProcessTarget->GetSrvIndex();
		renderTargetIndices.push_back(postProcessTargetIdx);
	}
	auto finalTarget = RESOURCE.Get<CTexture>("FinalTarget");
	if (finalTarget) {
		int finalTargetIdx = finalTarget->GetSrvIndex();
		renderTargetIndices.push_back(finalTargetIdx);
	}
	
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
	auto& lightCamera = mCameras["DirectionalLight"];
	auto& mainCamera = mCameras["MainCamera"];
	if (!lightCamera) {
		return;
	}
	INSTANCE(CDX12Manager).PrepareShadowPass();

	auto shadowPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);

	auto offset = ALIGNED_SIZE(sizeof(CBPassData));
	shadowPassBuffer->BindToShader(offset);

	lightCamera->SetViewportsAndScissorRects(CMDLIST);
	RenderForLayer("Opaque", mainCamera, SHADOW);

	auto shadowMap = RESOURCE.Get<CTexture>("ShadowMap");
	shadowMap->ChangeResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void CScene::RenderForwardPass()
{
	/*auto forwardPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	forwardPassBuffer->BindToShader(0);

	auto& camera = mCameras["MainCamera"];
	if (camera) {
		RenderForLayer("Transparent", camera, FORWARD);
	}*/
}

void CScene::RenderGBufferPass()
{
	auto renderTarget = RT_GROUP(RENDER_TARGET_GROUP_TYPE::G_BUFFER);
	renderTarget->ChangeResourcesToTargets();
	renderTarget->SetRenderTargets();
	renderTarget->ClearRenderTargets();
	renderTarget->ClearDepthStencil();
	auto gBufferPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	gBufferPassBuffer->BindToShader(0);
	auto& camera = mCameras["MainCamera"];
	auto& lightCamera = mCameras["DirectionalLight"];
	if (camera) {
		camera->SetViewportsAndScissorRects(CMDLIST);
		RenderForLayer("Opaque", lightCamera, G_PASS);
		if (mTerrain) mTerrain->Render(camera, G_PASS);
	}

	renderTarget->ChangeTargetsToResources();
}

void CScene::RenderLightingPass()
{
	auto lightingPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	lightingPassBuffer->BindToShader(0);
	auto renderTarget = RT_GROUP(RENDER_TARGET_GROUP_TYPE::LIGHTING_PASS);
	renderTarget->ChangeResourcesToTargets();
	renderTarget->SetRenderTargets();
	renderTarget->ClearRenderTargets();
	renderTarget->ClearOnlyStencil(0);
	auto& directionalLight = mLights[(UINT)LIGHT_TYPE::DIRECTIONAL][0];
	auto& pointLights = mLights[(UINT)LIGHT_TYPE::POINT];
	auto& spotLights = mLights[(UINT)LIGHT_TYPE::SPOT];
	auto& camera = mCameras["MainCamera"];

	UINT lightCount = 1 + pointLights.size() + spotLights.size();
	auto lightBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::LIGHT);
	lightBuffer->UpdateBuffer(sizeof(CBLightsData) * 10, &lightCount);
	lightBuffer->BindToShader(0);
	camera->SetViewportsAndScissorRects(CMDLIST);
	//Directional Light
	if (directionalLight) {
		directionalLight->Render(renderTarget);
	}
	//Point Light
	CMDLIST->OMSetStencilRef(0);
	for (const auto& pointLight : pointLights) {
		pointLight->Render(renderTarget);
	}
	//Spot Light
	for (const auto& spotLight : spotLights) {
		spotLight->Render(renderTarget);
	}
	if (camera) {
		RenderForLayer("Sky", camera, FORWARD);
		RenderForLayer("Transparent", camera, FORWARD);
	}
	renderTarget->ChangeTargetsToResources();
}

void CScene::RenderFinalPass()
{
	auto finalPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	finalPassBuffer->BindToShader(0);
	auto renderTarget = RT_GROUP(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN);
	UINT backBufferIdx = INSTANCE(CDX12Manager).GetCurrBackBufferIdx();
	renderTarget->ChangeResourceToTarget(backBufferIdx);
	renderTarget->SetRenderTarget(backBufferIdx);
	renderTarget->ClearRenderTarget(backBufferIdx);

	auto& camera = mCameras["MainCamera"];
	camera->SetViewportsAndScissorRects(CMDLIST);
	auto finalShader = RESOURCE.Get<CShader>("FinalPass");
	if(finalShader) {
		finalShader->SetPipelineState(CMDLIST);
		CRenderer::RenderFullscreen();
	}
	RenderForLayer("UI", camera);

	renderTarget->ChangeTargetToResource(backBufferIdx);
}

void CScene::LoadSceneFromFile(const std::string& fileName)
{
	std::ifstream ifs{ fileName, std::ios::binary };
	if (!ifs) {
		return;
	}
	//ÇÁ¸®ÆÕ
	std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs = RESOURCE.GetPrefabs();
	CreatePrefabs(ifs, prefabs);

	int rootNum{};
	BinaryReader::ReadDateFromFile(ifs, rootNum);
	for (int i = 0; i < rootNum; i++) {
		auto object = CGameObject::CreateObjectFromFile(ifs, prefabs);
		auto& tag = object->GetTag();
		if (tag == "Water") AddObject("Transparent", object);
		else if (tag == "UI") AddObject("UI", object);
		else if (tag == "SkyDome") AddObject("Sky", object);
		else AddObject("Opaque", object);
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

	auto type = object->GetObjectType();
	if (type != OBJECT_TYPE::NONE) {
		auto itr = std::find_if(mObjectTypes[type].begin(), mObjectTypes[type].end(),
			[object](const std::shared_ptr<CGameObject>& ptr) { return ptr == object; });
		if (itr == mObjectTypes[type].end()) {
			mObjectTypes[type].push_back(object);
		}
	}
	
	auto& objectList = mRenderLayers[renderLayer];
	auto itr1 = findByRawPointer(objectList, object.get());
	if (itr1 == objectList.end()) {
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

	mCameras[tag] = camera;
}

void CScene::RemoveCamera(const std::string& tag)
{
	if (!mCameras.contains(tag)) {
		return;
	}

	mCameras.erase(tag);
}

void CScene::AddLight(std::shared_ptr<CLight> light)
{
	auto& tag = light->GetOwner()->GetTag();
	auto type = light->GetLightType();
	mLights[(UINT)type].push_back(light);
}

void CScene::RenderForLayer(const std::string& layer, std::shared_ptr<CCamera> camera, int pass)
{
	if (!mRenderLayers.contains(layer)) {
		return;
	}
	for (const auto& object : mRenderLayers[layer]) {
		object->Render(camera, pass);
	}
}

void CScene::UpdatePassData()
{
	CBPassData passData;
	Matrix T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	);

	auto& camera = mCameras["MainCamera"];
	auto& lightCamera = mCameras["DirectionalLight"];

	if (camera) {
		passData.camPos = camera->GetLocalPosition();
		passData.viewProjMat = camera->GetViewProjMat().Transpose();
		passData.viewMat = camera->GetViewMat().Transpose();

		if (lightCamera) {
			passData.shadowTransform = (lightCamera->GetViewOrthoProjMat() * T).Transpose();
			passData.shadowMapIdx = RESOURCE.Get<CTexture>("ShadowMap")->GetSrvIndex();
		}
	}
	passData.deltaTime = DELTA_TIME;
	passData.totalTime = TIMER.GetTotalTime();
	passData.renderTargetSize = INSTANCE(CDX12Manager).GetRenderTargetSize();

	UIProjectionMatrix = XMMatrixOrthographicOffCenterLH(
		0.f, passData.renderTargetSize.x,
		passData.renderTargetSize.y, 0.f,
		0.f, 1.f
	);
	passData.uiTransform = UIProjectionMatrix.Transpose();
	
	int idx = 0;
	passData.gbufferAlbedoIdx = renderTargetIndices[idx++];
	passData.gbufferNormalIdx = renderTargetIndices[idx++];
	passData.gbufferEmissiveIdx = renderTargetIndices[idx++];
	passData.gbufferPosIdx = renderTargetIndices[idx++];
	passData.gbufferDepthIdx = renderTargetIndices[idx++];
	auto lightingTarget = RESOURCE.Get<CTexture>("LightingTarget");
	if (lightingTarget) {
		passData.lightingTargetIdx = renderTargetIndices[renderPasstype];
	}
	auto postProcessTarget = RESOURCE.Get<CTexture>("PostProcessTarget");
	if (postProcessTarget) {
		passData.postProcessIdx = postProcessTarget->GetSrvIndex();
	}
	auto finalTarget = RESOURCE.Get<CTexture>("FinalTarget");
	if (finalTarget) {
		passData.finalTargetIdx = finalTarget->GetSrvIndex();
	}

	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::PASS)->UpdateBuffer(0, &passData, sizeof(CBPassData));

	if (lightCamera) {
		passData.camPos = lightCamera->GetLocalPosition();
		passData.viewMat = lightCamera->GetViewMat().Transpose();
		passData.viewProjMat = lightCamera->GetViewOrthoProjMat().Transpose();
	}

	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::PASS)->UpdateBuffer(ALIGNED_SIZE(sizeof(CBPassData)), &passData, sizeof(CBPassData));
}
