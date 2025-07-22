#include "stdafx.h"
#include "RenderManager.h"
#include"InstancingGroup.h"
#include"InstancingBuffer.h"
#include"ResourceManager.h"
#include"Shader.h"
#include"Camera.h"
#include"Mesh.h"
#include"Material.h"
#include"Renderer.h"
#include"MeshRenderer.h"
#include"Light.h"
#include"DX12Manager.h"
#include"ShadowManager.h"
#include"ParticleManager.h"
#include"Terrain.h"

void CRenderManager::Initialize()
{
	mInstancingBufferOffset = 0;
}

void CRenderManager::Destroy()
{
	mInstancingGroupMap.clear();
	mInstancingBufferOffset = 0;
	for(RENDER_LAYER layer = RENDER_LAYER::Opaque; layer < RENDER_LAYER::End; layer = (RENDER_LAYER)((UINT)layer + 1)) {
		ClearRenderLayer(layer);
	}
	for(auto& lights : mLights) {
		lights.clear();
	}
}

void CRenderManager::AddRenderer(CRenderer* renderer, RENDER_LAYER layer)
{
	if (!renderer) return;
	auto& renderers = mRenderLayerLists[(UINT)layer];
	renderers.push_back(renderer);
}

void CRenderManager::RenderLayer(int pass, RENDER_LAYER layer, CCamera* camera)
{
	auto& renderers = mRenderLayerLists[(UINT)layer];
	for (auto& renderer : renderers) {
		if (!renderer) continue;
		renderer->Render(camera, pass);
	}
}

void CRenderManager::AddLight(CLight* light)
{
	if (!light) return;
	auto& lights = mLights[(UINT)light->GetLightType()];
	lights.push_back(light);
}

void CRenderManager::RemoveLight(CLight* light)
{
	if (!light) return;
	auto& lights = mLights[(UINT)light->GetLightType()];
	auto it = std::find(lights.begin(), lights.end(), light);
	if (it != lights.end()) {
		lights.erase(it);
	}
}

void CRenderManager::AddInstancingGroup(const InstanceKey& key, std::shared_ptr<class CInstancingGroup> group)
{
	mInstancingGroupMap[key] = group;
	key.material->EnrollToPool();
}

void CRenderManager::AddInstancingObject(const InstanceKey& key, std::shared_ptr<class CGameObject> object)
{
	if (!object) return;
	auto mesh = key.mesh;
	auto material = key.material;
	if (!mesh || !material) return;
	auto group = FindInstancingGroup(key);
	
	if (!group) {
		group = std::make_shared<CInstancingGroup>();
		group->Initialize(INSTANCE_BUFFER_TYPE::OBJECT);
		AddInstancingGroup(key, group);
	}
	group->AddObject(object);
}

void CRenderManager::AddInstancingObject(std::shared_ptr<class CGameObject> object)
{
	if (!object) return;
	auto meshRenderer = object->GetComponent<CMeshRenderer>();
	if (!meshRenderer) return;
	auto key = meshRenderer->GetInstancingKey();
	auto mesh = key.mesh;
	auto material = key.material;
	if (!mesh || !material) return;
	auto group = FindInstancingGroup(key);
	if (!group) {
		group = std::make_shared<CInstancingGroup>();
		group->Initialize(INSTANCE_BUFFER_TYPE::OBJECT);
		AddInstancingGroup(key, group);
	}
	group->AddObject(object);
}

void CRenderManager::UpdateInstancingGroup()
{
	auto& camera = mCameras["MainCamera"];
	mInstancingBufferOffset = 0;
	for (auto& [key, group] : mInstancingGroupMap) {
		mInstancingBufferOffset = group->Update(camera, mInstancingBufferOffset);
		key.material->Update();
	}
}

void CRenderManager::RenderShadowPass()
{
	INSTANCE(CDX12Manager).PrepareShadowPass();
	INSTANCE(CShadowManager).RenderShadowMaps();

	auto shadowMap = RESOURCE.Get<CTexture>("ShadowMap");
	shadowMap->ChangeResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void CRenderManager::RenderForwardPass()
{
	/*auto forwardPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	forwardPassBuffer->BindToShader(0);

	auto& camera = mCameras["MainCamera"];
	if (camera) {
		RenderForLayer("Transparent", camera, FORWARD);
	}*/
}

void CRenderManager::RenderGBufferPass()
{
	auto renderTarget = RT_GROUP(RENDER_TARGET_GROUP_TYPE::G_BUFFER);
	renderTarget->ChangeResourcesToTargets();
	renderTarget->SetRenderTargets();
	renderTarget->ClearRenderTargets();
	renderTarget->ClearDepthStencil(0.f, 0);
	auto gBufferPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	gBufferPassBuffer->BindToShader(0);
	INSTANCE(CShadowManager).BindShadowData();
	auto& camera = mCameras["MainCamera"];
	if (camera) {
		camera->SetViewportsAndScissorRects(CMDLIST);
		RenderLayer(G_PASS, RENDER_LAYER::Opaque, camera);
		RenderInstancingGroup(G_PASS);
		if (mTerrain) mTerrain->Render(camera, G_PASS);
	}

	renderTarget->ChangeTargetsToResources();
}

void CRenderManager::RenderLightingPass()
{
	auto lightingPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	lightingPassBuffer->BindToShader(0);
	auto renderTarget = RT_GROUP(RENDER_TARGET_GROUP_TYPE::LIGHTING_PASS);
	renderTarget->ChangeResourcesToTargets();
	renderTarget->SetRenderTargets();
	renderTarget->ClearRenderTargets();
	renderTarget->ClearOnlyStencil(0);

	CLight* directionalLight{};
	if (!mLights[(UINT)LIGHT_TYPE::DIRECTIONAL].empty()) directionalLight = mLights[(UINT)LIGHT_TYPE::DIRECTIONAL][0];
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
	for (const auto& pointLight : pointLights) {
		pointLight->Render(renderTarget);
	}
	//Spot Light
	for (const auto& spotLight : spotLights) {
		spotLight->Render(renderTarget);
	}
	if (camera) {
		RenderLayer(FORWARD, RENDER_LAYER::Transparent, camera);
		INSTANCE(CParticleManager).Render();
	}
	renderTarget->ChangeTargetsToResources();
}

void CRenderManager::RenderUIPass(bool clearRenderTarget)
{
	auto uiPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	uiPassBuffer->BindToShader(0);
	auto renderTarget = RT_GROUP(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN);
	UINT backBufferIdx = INSTANCE(CDX12Manager).GetCurrBackBufferIdx();
	renderTarget->ChangeResourceToTarget(backBufferIdx);
	renderTarget->SetRenderTarget(backBufferIdx);
	if (clearRenderTarget) {
		renderTarget->ClearRenderTargets();
	}
	Vec2 rtSize = INSTANCE(CDX12Manager).GetRenderTargetSize();

	D3D12_VIEWPORT mViewport = { 0.f,0.f,rtSize.x, rtSize.y };
	D3D12_RECT mScissorRect = { 0.f,0.f,rtSize.x, rtSize.y };

	CMDLIST->RSSetViewports(1, &mViewport);
	CMDLIST->RSSetScissorRects(1, &mScissorRect);
	RenderLayer(FORWARD, RENDER_LAYER::UI, nullptr);
	renderTarget->ChangeTargetToResource(backBufferIdx);
}

void CRenderManager::RenderFinalPass()
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
	if (finalShader) {
		finalShader->SetPipelineState(CMDLIST);
		CRenderer::RenderFullscreen();
	}

	renderTarget->ChangeTargetToResource(backBufferIdx);
}

void CRenderManager::RenderFadePass()
{
	auto fadePassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	fadePassBuffer->BindToShader(0);
	auto renderTarget = RT_GROUP(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN);
	UINT backBufferIdx = INSTANCE(CDX12Manager).GetCurrBackBufferIdx();
	renderTarget->ChangeResourceToTarget(backBufferIdx);
	renderTarget->SetRenderTarget(backBufferIdx);
	Vec2 rtSize = INSTANCE(CDX12Manager).GetRenderTargetSize();
	D3D12_VIEWPORT mViewport = { 0.f,0.f,rtSize.x, rtSize.y };
	D3D12_RECT mScissorRect = { 0.f,0.f,rtSize.x, rtSize.y };
	CMDLIST->RSSetViewports(1, &mViewport);
	CMDLIST->RSSetScissorRects(1, &mScissorRect);
	auto fadeShader = RESOURCE.Get<CShader>("FadeInOut");
	if (fadeShader) {
		fadeShader->SetPipelineState(CMDLIST);
		CRenderer::RenderFullscreen();
	}
	renderTarget->ChangeTargetToResource(backBufferIdx);
}

void CRenderManager::RenderCircularFadePass()
{
	auto fadePassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	fadePassBuffer->BindToShader(0);
	auto renderTarget = RT_GROUP(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN);
	UINT backBufferIdx = INSTANCE(CDX12Manager).GetCurrBackBufferIdx();
	renderTarget->ChangeResourceToTarget(backBufferIdx);
	renderTarget->SetRenderTarget(backBufferIdx);
	Vec2 rtSize = INSTANCE(CDX12Manager).GetRenderTargetSize();
	D3D12_VIEWPORT mViewport = { 0.f,0.f,rtSize.x, rtSize.y };
	D3D12_RECT mScissorRect = { 0.f,0.f,rtSize.x, rtSize.y };
	CMDLIST->RSSetViewports(1, &mViewport);
	CMDLIST->RSSetScissorRects(1, &mScissorRect);
	auto fadeShader = RESOURCE.Get<CShader>("FadeInOutCircle");
	if (fadeShader) {
		fadeShader->SetPipelineState(CMDLIST);
		CRenderer::RenderFullscreen();
	}
	renderTarget->ChangeTargetToResource(backBufferIdx);
}

void CRenderManager::RenderInstancingGroup(int pass)
{
	auto instancingBuffer = INSTANCE(CDX12Manager).GetInstancingBuffer((UINT)INSTANCE_BUFFER_TYPE::OBJECT);
	for (auto& [key, group] : mInstancingGroupMap) {
		int instancingCount = group->mInstancingCnt;
		if (instancingCount == 0) continue;

		auto mesh = key.mesh;
		auto material = key.material;
		if (!mesh || !material) continue;
		if (!material->GetShader((PASS_TYPE)pass)) continue;
		if (mesh->GetSubMeshNum() == 0) continue;
		material->BindShader((PASS_TYPE)pass);
		material->BindDataToShader();

		int startOffset = group->mInstancingBufferOffset;
		auto instancingbufferView = instancingBuffer->GetInstancingBufferView(startOffset, instancingCount);

		mesh->Render(instancingbufferView, instancingCount, 0);
	}
}
