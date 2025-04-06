#include "stdafx.h"
#include "ResourceManager.h"
#include"DX12Manager.h"
#include"Mesh.h"
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

	int materialCount{};
	ReadDateFromFile(ifs, materialCount);
	for (int i = 0; i < materialCount; i++) {
		Add(CMaterial::CreateMaterialFromFile(ifs));
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
}

void CResourceManager::LoadDefaultMaterials()
{
}

void CResourceManager::LoadDefaultShaders()
{
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, "Common");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::SHADOW;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, "Common");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, "Triplanar");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::SHADOW;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, "Triplanar");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, "Vegitation");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::SHADOW;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, "Vegitation");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, "Sprite");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::TERRAIN;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, "Terrain");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, "Skybox");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = PASS_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::BILLBOARD;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, "Billboard");

		Add(shader);
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



