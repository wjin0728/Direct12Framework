#include "stdafx.h"
#include "ResourceManager.h"
#include"DX12Manager.h"
#include"Mesh.h"
#include"SkinnedMesh.h"
#include"Material.h"
#include"Shader.h"


void CResourceManager::Initialize()
{
	for (UINT i = 0; i < TEXTURE_COUNT; i++) {
		srvIdxQueue.push(i);
	}
	LoadDefaultMeshes();
	LoadDefaultTexture();
	LoadDefaultShaders();
	LoadDefaultMaterials();

	LoadSceneResourcesFromFile("..\\Resources\\Scenes\\LobbyResources.bin");

	LoadPlayerObjects();
	LoadSkillObjects();
}

void CResourceManager::Destroy()
{
	for (auto& map : resources) {
		map.clear();
	}
}

std::shared_ptr<CTexture> CResourceManager::Create2DTexture(const std::string& name, DXGI_FORMAT format
	, void* data, size_t dataSize, UINT width, UINT height,
	const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_FLAGS resFlags, XMFLOAT4 clearColor)
{
	KeyObjMap& keyObjMap = resources[static_cast<UINT8>(RESOURCE_TYPE::TEXTURE)];

	auto itr = keyObjMap.find(name);
	if (itr != keyObjMap.end())
		return std::static_pointer_cast<CTexture>(itr->second);

	std::shared_ptr<CTexture> m = std::make_shared<CTexture>();
	m->Create2DTexture(format, data, dataSize, width, height, heapProperty, heapFlags, resFlags, clearColor);

	keyObjMap[name] = m;

	return m;
}

void CResourceManager::UpdateMaterials()
{
	KeyObjMap& keyObjMap = resources[static_cast<UINT8>(RESOURCE_TYPE::MATERIAL)];

	for (auto& [key, material] : keyObjMap) {
		static_pointer_cast<CMaterial>(material)->Update();
	}
}

void CResourceManager::LoadSceneResourcesFromFile(std::ifstream& ifs)
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
}

void CResourceManager::LoadSceneResourcesFromFile(const std::string& fileName)
{
	std::ifstream ifs{ fileName, std::ios::binary };
	if (!ifs) {
		return;
	}
	LoadSceneResourcesFromFile(ifs);
}

void CResourceManager::LoadPlayerObjects()
{
	LoadPrefabFromFile("Player_Archer");
	LoadPrefabFromFile("Player_Fighter");
	LoadPrefabFromFile("Player_Mage");
}

void CResourceManager::LoadSkillObjects()
{
	LoadPrefabFromFile("Item_Skill1");
	LoadPrefabFromFile("Item_Skill2");
	LoadPrefabFromFile("Item_Skill3");
}

void CResourceManager::LoadPrefabFromFile(const std::string& name)
{
	auto skill = CGameObject::CreateObjectFromFile(name);
	if (skill) {
		skill->SetName(name);
		skill->SetActive(false);
		skill->SetStatic(false);
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
	{
		std::shared_ptr<CMesh> m = CMesh::CreateRectangleMesh({2.f,2.f});
		m->SetName("Rectangle");
		m->SetType(RESOURCE_TYPE::MESH);
		Add(m);
	}


	auto& meshes = resources[static_cast<UINT>(RESOURCE_TYPE::MESH)];

	for (auto& mesh : meshes) {
		static_pointer_cast<CMesh>(mesh.second)->CreateVertexBuffer();
		static_pointer_cast<CMesh>(mesh.second)->CreateIndexBuffers();
	}
}

void CResourceManager::LoadDefaultTexture()
{
	auto shadowMap = Get<CTexture>("ShadowMap");
	shadowMap->CreateSRV();
	shadowMap->ChangeResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);

	auto albedo = Get<CTexture>("GBufferAlbedo");
	albedo->CreateSRV();
	auto normal = Get<CTexture>("GBufferNormal");
	normal->CreateSRV();
	auto emissive = Get<CTexture>("GBufferEmissive");
	emissive->CreateSRV();
	auto position = Get<CTexture>("GBufferPosition");
	position->CreateSRV();
	auto depth = Get<CTexture>("GBufferDepth");
	depth->CreateSRV();
	auto lighting = Get<CTexture>("LightingTarget");
	lighting->CreateSRV();
	auto postProcess = Get<CTexture>("PostProcessTarget");
	postProcess->CreateSRV();
	auto final = Get<CTexture>("FinalTarget");
	final->CreateSRV();
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

		MakeShadersForAllPass("Common", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		MakeShadersForAllPass("Triplanar", info);

	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		MakeShadersForAllPass("Vegitation", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		MakeShadersForAllPass("Sprite", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::TERRAIN;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		MakeShadersForAllPass("Terrain", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		MakeShadersForAllPass("Skybox", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::BILLBOARD;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		MakeShadersForAllPass("Billboard", info);
	}
	{
		ShaderInfo info;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::ANIMATION;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		MakeShadersForAllPass("Animation", info);
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
		if (shader->Initialize(info, "Lighting")) Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::STENCIL;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::GREATER;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		if (shader->Initialize(info, "Lighting")) Add(shader);
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
		if (shader->Initialize(info, "Lighting")) Add(shader);
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
		if (shader->Initialize(info, "FinalPass", false)) Add(shader);
	}
}

void CResourceManager::MakeShadersForAllPass(const std::string& name, ShaderInfo info)
{
	for (UINT8 i = 0; i < static_cast<UINT8>(PASS_TYPE::STENCIL); i++) {
		info.shaderType = static_cast<PASS_TYPE>(i);
		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		if (shader->Initialize(info, name)) Add(shader);
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



