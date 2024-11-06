#include "stdafx.h"
#include"DX12Manager.h"
#include "MeshRenderer.h"
#include"ResourceManager.h"
#include"Mesh.h"
#include"Material.h"
#include"Camera.h"

CMeshRenderer::CMeshRenderer() : CComponent(COMPONENT_TYPE::MESH_RENDERER)
{
}


CMeshRenderer::~CMeshRenderer()
{
}

void CMeshRenderer::Awake()
{
}

void CMeshRenderer::Start()
{
}

void CMeshRenderer::Update()
{
}

void CMeshRenderer::LateUpdate()
{
}

void CMeshRenderer::Render()
{
	int subMeshNum = m_mesh->GetSubMeshNum();

	for (int i = 0; i < subMeshNum; i++) {
		m_mesh->Render(CMDLIST, i);
	}
}

void CMeshRenderer::SetMesh(const std::wstring& name)
{
	m_mesh = INSTANCE(CResourceManager).Get<CMesh>(name);
}

void CMeshRenderer::AddMaterial(const std::shared_ptr<CMaterial>& material)
{
	m_materials.push_back(material);
}

void CMeshRenderer::AddMaterial(const std::wstring& name)
{
	m_materials.push_back(INSTANCE(CResourceManager).Get<CMaterial>(name));
}

int CMeshRenderer::GetMaterialIndex() const
{
	return m_materials[0]->mSrvIdx;
}
