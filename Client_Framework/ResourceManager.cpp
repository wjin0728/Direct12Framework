#include "stdafx.h"
#include "ResourceManager.h"
#include"DX12Manager.h"
#include"Mesh.h"
#include"SkinnedMesh.h"
#include"Material.h"
#include"Shader.h"
#include"UIRenderer.h"
#include"GameObject.h"
#include"ItemMovement.h"


void CResourceManager::Initialize()
{
	for (UINT i = 0; i < TEXTURE_COUNT; i++) {
		srvIdxQueue.push(i);
	}
}

void CResourceManager::Destroy()
{
	if (mLoadThread.joinable())
	{
		mLoadThread.join(); // 스레드 종료 대기
	}
	for (auto& map : resources) {
		map.clear();
	}
}


void CResourceManager::UpdateMaterials()
{
	KeyObjMap& keyObjMap = resources[static_cast<UINT8>(RESOURCE_TYPE::MATERIAL)];

	for (auto& [key, material] : keyObjMap) {
		static_pointer_cast<CMaterial>(material)->Update();
	}
}

bool CResourceManager::LoadSceneResourcesFromFile(std::ifstream& ifs)
{
	using namespace BinaryReader;

	std::string token{};

	int meshCount{};
	ReadDateFromFile(ifs, meshCount);
	for (int i = 0; i < meshCount;i++) {
		std::string meshName;
		ReadDateFromFile(ifs, meshName);

		if (!Get<CMesh>(meshName)) {
			Add(CMesh::CreateMeshFromFile(meshName));
		}	
	}

	ReadDateFromFile(ifs, meshCount);
	for (int i = 0; i < meshCount; i++) {
		std::string meshName;
		ReadDateFromFile(ifs, meshName);

		if (!Get<CSkinnedMesh>(meshName)) {
			Add(CSkinnedMesh::CreateSkinnedMeshFromFile(meshName));
		}
	}

	int materialCount{};
	ReadDateFromFile(ifs, materialCount);
	for (int i = 0; i < materialCount; i++) {
		Add(CMaterial::CreateMaterialFromFile(ifs));
	}

	return true;
}

bool CResourceManager::LoadSceneResourcesFromFile(const std::string& fileName)
{
	std::ifstream ifs{ fileName, std::ios::binary };
	if (!ifs) {
		return false;
	}
	return LoadSceneResourcesFromFile(ifs);
}

bool CResourceManager::LoadPlayerObjects()
{
	LoadPrefabFromFile("Player_Archer");
	LoadPrefabFromFile("Player_Fighter");
	LoadPrefabFromFile("Player_Mage");

	return true;
}

bool CResourceManager::LoadSkillObjects()
{
	LoadPrefabFromFile("Item_Skill1");
	LoadPrefabFromFile("Item_Skill2");
	LoadPrefabFromFile("Item_Skill3");

	return true;
}

void CResourceManager::LoadPrefabFromFile(const std::string& name)
{
	std::ifstream ifs{ OBJECT_PATH(name), std::ios::binary };
	if (!ifs) {
		return;
	}
	using namespace BinaryReader;
	std::string token{};

	int meshCount{};
	ReadDateFromFile(ifs, meshCount);
	for (int i = 0; i < meshCount; i++) {
		std::string meshName;
		ReadDateFromFile(ifs, meshName);
		if (!Get<CMesh>(meshName)) {
			Add(CMesh::CreateMeshFromFile(meshName));
		}
	}
	int skinnedMeshCount{};
	ReadDateFromFile(ifs, skinnedMeshCount);
	for (int i = 0; i < skinnedMeshCount; i++) {
		std::string meshName;
		ReadDateFromFile(ifs, meshName);
		if (!Get<CSkinnedMesh>(meshName)) {
			Add(CSkinnedMesh::CreateSkinnedMeshFromFile(meshName));
		}
	}

	int materialCount{};
	ReadDateFromFile(ifs, materialCount);
	for (int i = 0; i < materialCount; i++) {
		Add(CMaterial::CreateMaterialFromFile(ifs));
	}
	auto skill = CGameObject::CreateObjectFromFile(ifs, prefabs);
	if (skill) {
		skill->SetName(name);
		skill->SetActive(false);
		skill->SetStatic(false);
		skill->SetInstancing(false);
		skill->SetObjectType(OBJECT_TYPE::ITEM);

		prefabs[name] = skill;
	}
}

void CResourceManager::LoadDefaultMeshes()
{
	{
		std::shared_ptr<CMesh> m = CMesh::CreateCubeMesh({ 1.f,1.f,1.f });
		m->SetName("Cube");
		m->SetType(RESOURCE_TYPE::MESH);
		Add(m);
	}
}

void CResourceManager::LoadDefaultTexture()
{
	auto& textures = resources[static_cast<UINT>(RESOURCE_TYPE::TEXTURE)];
	for (auto& texture : textures) {
		auto tex = static_pointer_cast<CTexture>(texture.second);
		tex->AssignedSRVIndex();
		tex->CreateSRV();
	}

	auto shadowMap = Get<CTexture>("ShadowMap");
	INSTANCE(CDX12Manager).OpenCommandList();
	shadowMap->ChangeResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
	INSTANCE(CDX12Manager).CloseCommandList();
}

void CResourceManager::LoadDefaultMaterials()
{
}

void CResourceManager::LoadDefaultShaders()
{
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		MakeShadersForAllPass("Common", "Common", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		MakeShadersForAllPass("LitOpaque", "Lit", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::ALPHA_BLEND;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		MakeShadersForAllPass("LitTransparent", "Lit", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		MakeShadersForAllPass("Triplanar", "Triplanar", info);

	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		MakeShadersForAllPass("Vegitation", "Vegitation", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::TERRAIN;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		MakeShadersForAllPass("Terrain", "Terrain", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		MakeShadersForAllPass("Skybox", "Skybox", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::ANIMATION;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		MakeShadersForAllPass("LitOpaqueAnimation", "Lit", info);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::DIRECTIONAL;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::NONE;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_NO_WRITE;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		if (shader->Initialize("Lighting", info, "Lighting")) Add(shader);
	} {
		ShaderInfo info;
		info.shaderType = PASS_TYPE::DIRECTIONAL;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::NONE;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_NO_WRITE;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		if (shader->Initialize("Lighting", info, "Lighting")) Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::ALPHA_BLEND;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		if (shader->Initialize("Water", info, "Water")) Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::LIGHTING;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::ADD_BLEND;
		info.depthStencilType = DEPTH_STENCIL_TYPE::GREATER_EQUAL;
		info.rasterizerType = RASTERIZER_TYPE::CULL_FRONT;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		if (shader->Initialize("Lighting", info, "Lighting")) Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::FINAL;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::NONE;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		if (shader->Initialize("FinalPass", info, "FinalPass", false)) Add(shader);
	}
	
}

bool CResourceManager::LoadLoadingScreen()
{
	{
		std::shared_ptr<CMesh> m = CMesh::CreateRectangleMesh({ 2.f,2.f });
		m->SetName("Rectangle");
		m->SetType(RESOURCE_TYPE::MESH);
		Add(m);
		CUIRenderer::mQuad = m;
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::UI;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::ALPHA_BLEND;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		if (shader->Initialize("Sprite", info, "Sprite", false)) Add(shader);
	}
	for (int i = 0; i < 16;i++) {
		auto name = "Loading" + std::to_string(i);
		std::shared_ptr<CTexture> loadingScreen = std::make_shared<CTexture>(name, TEXTURE_PATH(name));
		loadingScreen->AssignedSRVIndex();
		Add(loadingScreen);
	}

	return true;
}

void CResourceManager::MakeShadersForAllPass(const std::string& shaderName, const std::string& name, ShaderInfo info)
{
	for (UINT8 i = 0; i < static_cast<UINT8>(PASS_TYPE::STENCIL); i++) {
		info.shaderType = static_cast<PASS_TYPE>(i);
		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		if (shader->Initialize(shaderName, info, name)) Add(shader);
	}

}

void CResourceManager::ReleaseUploadBuffers()
{
	for (auto& map : resources) {
		for (auto& [name, resource] : map) {
			resource->ReleaseUploadBuffer();
		}
	}
}

UINT CResourceManager::GetTopSRVIndex()
{
	UINT idx = srvIdxQueue.front();
	srvIdxQueue.pop();

	return idx;
}

UINT CResourceManager::GetMaterialSRVIndex()
{
	UINT idx = resources[static_cast<UINT8>(RESOURCE_TYPE::MATERIAL)].size();
	return idx;
}

void CResourceManager::BackgroundLoadingThread()
{
	LoadDefaultMeshes();
	LoadDefaultShaders();
	LoadDefaultMaterials();
	LoadSceneResourcesFromFile("..\\Resources\\Scenes\\Battle1Resources.bin");
	LoadPlayerObjects();
	LoadSkillObjects();

	mLoadFinished = true;
	mThreadRunning = false;
}

void CResourceManager::EnqueueRequest(CResource* req)
{
	{
		std::lock_guard<std::mutex> lock(mQueueMutex);
		mGPULoadQueue.push(req);
	}
}

void CResourceManager::ProcessGPULoadImmediate(CResource* req)
{
	INSTANCE(CDX12Manager).OpenCommandList();
	req->CreateGPUResource();
	INSTANCE(CDX12Manager).CloseCommandList();
	req->ReleaseUploadBuffer();
}

void CResourceManager::ProcessGPULoadQueue(int maxPerFrame)
{
	std::vector<CResource*> resourcesToLoad{};
	{
		std::lock_guard<std::mutex> lock(mQueueMutex);
		int count = 0;
		int maxCount = maxPerFrame ? maxPerFrame : mGPULoadQueue.size();

		while (!mGPULoadQueue.empty() && count < maxCount)
		{
			auto req = mGPULoadQueue.front();
			mGPULoadQueue.pop();
			resourcesToLoad.push_back(req);
			count++;
		}
	}

	INSTANCE(CDX12Manager).OpenCommandList();
	for (auto& req : resourcesToLoad) {
		req->CreateGPUResource();
	}
	INSTANCE(CDX12Manager).CloseCommandList();
	for (auto& req : resourcesToLoad) {
		req->ReleaseUploadBuffer();
	}
}

void CResourceManager::RequestLoad()
{
	mLoadFinished = false;
	mThreadRunning = true;
	mLoadThread = std::thread(&CResourceManager::BackgroundLoadingThread, this);
}

bool CResourceManager::IsLoadFinished() const
{
	return mLoadFinished;
}