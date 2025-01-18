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
	LoadDefaultMaterials();
	LoadDefaultShaders();
}

std::shared_ptr<CTexture> CResourceManager::Create2DTexture(const std::wstring& name, DXGI_FORMAT format
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

void CResourceManager::LoadDefaultMeshes()
{
	{
		std::shared_ptr<CMesh> m = CMesh::CreateCubeMesh({ 1.f,1.f,1.f });
		m->SetName(L"Cube");
		m->SetType(RESOURCE_TYPE::MESH);
		Add(m);
	}
	{
		std::shared_ptr<CMesh> m = CMesh::CreateRectangleMesh({2.f,2.f});
		m->SetName(L"Rectangle");
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
	std::vector<std::shared_ptr<CTexture>> textures{};
	{
		std::shared_ptr<CTexture> texture = std::make_shared<CTexture>();
		texture->LoadFromFile(L"Resources\\Textures\\Base_Texture.dds");
		texture->SetName(L"TerrainBase");
		Add(texture);
		textures.push_back(texture);
	}
	{
		std::shared_ptr<CTexture> texture = std::make_shared<CTexture>();
		texture->LoadFromFile(L"Resources\\Textures\\MainMenu.dds");
		texture->SetName(L"MainMenu");
		Add(texture);
		textures.push_back(texture);
	}
	{
		std::shared_ptr<CTexture> texture = std::make_shared<CTexture>();
		texture->LoadFromFile(L"Resources\\Textures\\scrolling.dds");
		texture->SetName(L"Scrolling");
		Add(texture);
		textures.push_back(texture);
	}
	{
		std::shared_ptr<CTexture> texture = std::make_shared<CTexture>();
		texture->LoadFromFile(L"Resources\\Textures\\Detail_Texture_7.dds");
		texture->SetName(L"TerrainDetail");
		Add(texture);
		textures.push_back(texture);
	}
	{
		std::shared_ptr<CTexture> texture = std::make_shared<CTexture>();
		texture->LoadFromFile(L"Resources\\Textures\\SkyBox_0.dds");
		texture->SetName(L"SkyBox");
		texture->SetTextureType(TEXTURECUBE);
		Add(texture);
		textures.push_back(texture);
	}
	{
		std::shared_ptr<CTexture> texture = std::make_shared<CTexture>();
		texture->LoadFromFile(L"Resources\\Textures\\tree01.dds");
		texture->SetName(L"Tree1");
		Add(texture);
		textures.push_back(texture);
	}
	{
		std::shared_ptr<CTexture> texture = std::make_shared<CTexture>();
		texture->LoadFromFile(L"Resources\\Textures\\tree02.dds");
		texture->SetName(L"Tree2");
		Add(texture);
		textures.push_back(texture);
	}
	{
		std::shared_ptr<CTexture> texture = std::make_shared<CTexture>();
		texture->LoadFromFile(L"Resources\\Textures\\tree03.dds");
		texture->SetName(L"Tree3");
		Add(texture);
		textures.push_back(texture);
	}

	for (auto& texture : textures) {
		texture->CreateSRV();
	}
}

void CResourceManager::LoadDefaultMaterials()
{
	{
		std::shared_ptr<CMaterial> mat = std::make_shared<CMaterial>();
		mat->SetName(L"MainMenu");
		mat->mDiffuseMapIdx = Get<CTexture>(L"MainMenu")->GetSrvIndex();
		Add(mat);
	}
	{
		std::shared_ptr<CMaterial> mat = std::make_shared<CMaterial>();
		mat->SetName(L"SkyBox");
		Add(mat);
	}
	{
		std::shared_ptr<CMaterial> mat = std::make_shared<CMaterial>();
		mat->SetName(L"Scrolling");
		mat->mDiffuseMapIdx = Get<CTexture>(L"Scrolling")->GetSrvIndex();
		Add(mat);
	}
	{
		std::shared_ptr<CMaterial> mat = std::make_shared<CMaterial>();
		mat->SetName(L"Tree1");
		mat->mDiffuseMapIdx = Get<CTexture>(L"Tree1")->GetSrvIndex();
		Add(mat);
	}
	{
		std::shared_ptr<CMaterial> mat = std::make_shared<CMaterial>();
		mat->SetName(L"Tree2");
		mat->mDiffuseMapIdx = Get<CTexture>(L"Tree2")->GetSrvIndex();
		Add(mat);
	}
	{
		std::shared_ptr<CMaterial> mat = std::make_shared<CMaterial>();
		mat->SetName(L"Tree3");
		mat->mDiffuseMapIdx = Get<CTexture>(L"Tree3")->GetSrvIndex();
		Add(mat);
	}
}

void CResourceManager::LoadDefaultShaders()
{
	{
		ShaderInfo info;
		info.shaderType = SHADER_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, L"Resources\\Shaders\\Lighting.hlsl");
		shader->SetName(L"Forward");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = SHADER_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, L"Resources\\Shaders\\Sprite.hlsl");
		shader->SetName(L"Sprite");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = SHADER_TYPE::TERRAIN;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, L"Resources\\Shaders\\Terrain.hlsl");
		shader->SetName(L"Terrain");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = SHADER_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, L"Resources\\Shaders\\SkyBox.hlsl");
		shader->SetName(L"SkyBox");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = SHADER_TYPE::FORWARD;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::BILLBOARD;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, L"Resources\\Shaders\\Billboard.hlsl");
		shader->SetName(L"Billboard");

		Add(shader);
	}
	{
		ShaderInfo info;
		info.shaderType = SHADER_TYPE::SHADOW;
		info.inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, L"Resources\\Shaders\\Shadow.hlsl");
		shader->SetName(L"Shadow");

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



