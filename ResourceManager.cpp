#include "stdafx.h"
#include "ResourceManager.h"
#include"DX12Manager.h"
#include"Texture.h"
#include"Mesh.h"


void CResourceManager::Initialize()
{
	LoadDefaultMeshes();
	LoadDefaultTexture();
	LoadDefaultMaterials();
	LoadDefaultShaders();
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
	
	for (auto& texture : textures) {
		static_pointer_cast<CTexture>(texture.second)->CreateSRV();
	}
}

void CResourceManager::LoadDefaultMaterials()
{
}

void CResourceManager::LoadDefaultShaders()
{
}



