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
#include"RenderManager.h"
#include"ShadowManager.h"
#include"ParticleManager.h"


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

	mRenderMgr = &INSTANCE(CRenderManager);
}

void CScene::Activate()
{
	for (const auto& object : mObjects) {
		object->SetActive(true);
	}
	mIsActive = true;
}

void CScene::Start()
{
	for (const auto& object : mObjects) {
		ExpandSceneAABB(object, mSceneAABB);
	}
	INSTANCE(CShadowManager).UpdateSceneBoundingBox(mSceneAABB);
}

void CScene::Update()
{
	FadeUpdate();
	for (const auto& object : mObjects) {
		object->Update();
	}
}

void CScene::LateUpdate() 
{
	for (const auto& object : mObjects) {
		object->LateUpdate();
	}
	INSTANCE(CRenderManager).UpdateInstancingGroup();
	INSTANCE(CShadowManager).Update();
	INSTANCE(CParticleManager).Update();
	UpdatePassData();

}



void CScene::RenderFadeOverlay()
{
	if (mFadeType == FadeType::None) return;
	mRenderMgr->RenderFadePass();
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
		AddObjectImmediately(object);
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

	return obj;
}

void CScene::ExpandSceneAABB(std::shared_ptr<CGameObject> obj, BoundingBox& sceneAABB)
{
	if (!obj) return;
	if( obj->GetActive() && obj->mCastShadow) {
		const BoundingBox& objAABB = obj->mWorldAABB;
		BoundingBox::CreateMerged(sceneAABB, sceneAABB, objAABB);
	} 

	for (const auto& child : obj->GetChildren())
	{
		ExpandSceneAABB(child, sceneAABB);
	}
}

void CScene::AddObjectImmediately(std::shared_ptr<CGameObject> object, bool activate)
{
	auto itr = findByRawPointer(mObjects, object.get());
	if (itr == mObjects.end()) {
		mObjects.push_back(object);
	}

	auto type = object->GetObjectType();
	if (type != OBJECT_TYPE::NONE) {
		auto itr = std::find_if(mObjectTypes[type].begin(), mObjectTypes[type].end(),
			[object](const std::shared_ptr<CGameObject>& ptr) { return ptr == object; });
		if (itr == mObjectTypes[type].end()) {
			mObjectTypes[type].push_back(object);
		}
	}
	object->SetActive(activate);
}

void CScene::AddObject(std::shared_ptr<CGameObject> object)
{
	if (!object) return;
	mAddQueue.push(object);
}

void CScene::RemoveObject(std::shared_ptr<CGameObject> object)
{
	auto itr = findByRawPointer(mObjects, object.get());
	if (itr != mObjects.end()) {
		mObjects.erase(itr);
	}

	auto type = object->GetObjectType();
	if (type != OBJECT_TYPE::NONE) {
		auto itr = std::find_if(mObjectTypes[type].begin(), mObjectTypes[type].end(),
			[object](const std::shared_ptr<CGameObject>& ptr) { return ptr == object; });
		if (itr != mObjectTypes[type].end()) {
			mObjectTypes[type].erase(itr);
		}
	}
}

void CScene::SetTerrain(std::shared_ptr<CTerrain> terrain)
{
	mTerrain = terrain;
}

void CScene::UpdatePassData()
{
	CBPassData passData;
	auto camera = mRenderMgr->GetCamera("MainCamera");

	if (camera) {
		passData.camPos = camera->GetLocalPosition();
		passData.viewProjMat = camera->GetViewProjMat().Transpose();
		passData.invViewMat = camera->GetInverseViewMat().Transpose();
		passData.viewMat = camera->GetViewMat().Transpose();
		passData.projectionParams = Vec4(camera->GetNear(), camera->GetFar(), camera->GetFov(), camera->GetAspect());
	}
	passData.deltaTime = DELTA_TIME;
	passData.totalTime = TIMER.GetTotalTime();
	passData.renderTargetSize = INSTANCE(CDX12Manager).GetRenderTargetSize();

	UIProjectionMatrix = XMMatrixOrthographicOffCenterLH(
		0.f, passData.renderTargetSize.x,
		passData.renderTargetSize.y, 0.f,
		0.f, 1.f
	);
	
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
	passData.fadeColor = mFadeColor;

	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::PASS)->UpdateBuffer(0, &passData, sizeof(CBPassData));
}

void CScene::CollectVisibleObjects()
{
	for (const auto& object : mObjects) {
		object->RegisterRenderer();
	}
}

void CScene::AddRemoveQueue(std::shared_ptr<CGameObject> object)
{
	if (object) {
		object->SetActive(false);
		auto itr = findByRawPointer(mObjects, object.get());
		mRemoveQueue.push(object);
	}
}

void CScene::AddComponentToStartQueue(CComponent* component)
{
	if (!component) return;
	mComponentStartQueue.push(component);
}

void CScene::CommitObjectChanges()
{
	while (!mAddQueue.empty()) {
		auto object = mAddQueue.front();
		mAddQueue.pop();
		AddObjectImmediately(object, true);
	}
	while (!mComponentStartQueue.empty()) {
		auto component = mComponentStartQueue.front();
		mComponentStartQueue.pop();
		component->Start();
	}
	RemoveObjects();
}

void CScene::FadeUpdate()
{
	if (mFadeType == FadeType::None) return;
	mFadeTime += DELTA_TIME;
	float alpha = 0.f;
	if (mFadeType == FadeType::In) {
		alpha = mFadeTime / mFadeDuration;
	} else if (mFadeType == FadeType::Out) {
		alpha = 1.f - (mFadeTime / mFadeDuration);
	}
	mFadeColor.w = std::clamp(alpha, 0.f, 1.f);
	if (mFadeTime >= mFadeDuration) {
		mFadeColor.w = (mFadeType == FadeType::In) ? 1.f : 0.f;
		mFadeType = FadeType::None;
		if (mOnFadeFinish) {
			mOnFadeFinish();
			mOnFadeFinish = nullptr;
		}
	}
}

void CScene::FadeIn(float duration, const Color& color, std::function<void()> onFinish)
{
	mFadeColor = color;
	mFadeDuration = duration;
	mFadeTime = 0.f;
	mOnFadeFinish = onFinish;
	mFadeColor.w = 0.f; 
	mFadeType = FadeType::In;
}

void CScene::FadeOut(float duration, const Color& color, std::function<void()> onFinish)
{
	mFadeColor = color;
	mFadeDuration = duration;
	mFadeTime = 0.f;
	mOnFadeFinish = onFinish;
	mFadeColor.w = 1.f; 
	mFadeType = FadeType::Out;
}

void CScene::RemoveObjects()
{
	while (!mRemoveQueue.empty()) {
		auto object = mRemoveQueue.front();
		mRemoveQueue.pop();
		RemoveObject(object);
	}
}




































































































































































































































