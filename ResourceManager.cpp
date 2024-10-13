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

std::shared_ptr<CTexture> CResourceManager::Create2DTexture(const std::wstring& name, DXGI_FORMAT format, UINT width, UINT height,
	const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_FLAGS resFlags, XMFLOAT4 clearColor)
{
	KeyObjMap& keyObjMap = resources[static_cast<UINT8>(RESOURCE_TYPE::TEXTURE)];

	auto itr = keyObjMap.find(name);
	if (itr != keyObjMap.end())
		return std::static_pointer_cast<CTexture>(itr->second);

	std::shared_ptr<CTexture> m = std::make_shared<CTexture>();
	m->Create2DTexture(format, width, height, heapProperty, heapFlags, resFlags, clearColor);

	keyObjMap[name] = m;

	return m;
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
		std::shared_ptr<CMesh> m = CMesh::CreateAlphabetMesh('S');
		m->SetName(L"S");
		m->SetType(RESOURCE_TYPE::MESH);
		Add(m);
	}
	{
		std::shared_ptr<CMesh> m = CMesh::CreateAlphabetMesh('T');
		m->SetName(L"T");
		m->SetType(RESOURCE_TYPE::MESH);
		Add(m);
	}
	{
		std::shared_ptr<CMesh> m = CMesh::CreateAlphabetMesh('A');
		m->SetName(L"A");
		m->SetType(RESOURCE_TYPE::MESH);
		Add(m);
	}
	{
		std::shared_ptr<CMesh> m = CMesh::CreateAlphabetMesh('R');
		m->SetName(L"R");
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
		static_pointer_cast<CMesh>(mesh.second)->CreateBufferViews();
	}
}

void CResourceManager::LoadDefaultTexture()
{
	auto& textures = resources[static_cast<UINT>(RESOURCE_TYPE::TEXTURE)];
	
	for (auto& [name, texture] : textures) {
		static_pointer_cast<CTexture>(texture)->CreateSRV();
	}
}

void CResourceManager::LoadDefaultMaterials()
{
	std::shared_ptr<CMaterial> mat = std::make_shared<CMaterial>();
}

void CResourceManager::LoadDefaultShaders()
{
	{
		ShaderInfo info;
		info.shaderType = SHADER_TYPE::FORWARD;
		info.blendType = BLEND_TYPE::DEFAULT;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		info.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		info.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		shader->Initialize(info, L"Resources\\Shaders\\Lighting.hlsl");
		shader->SetName(L"Forward");

		Add(shader);
	}
}

UINT CResourceManager::GetTopSRVIndex()
{
	UINT idx = srvIdxQueue.top();
	srvIdxQueue.pop();

	return idx;
}



