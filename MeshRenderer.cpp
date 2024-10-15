#include "stdafx.h"
#include "MeshRenderer.h"
#include"ResourceManager.h"
#include"Mesh.h"
#include"Material.h"

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

void CMeshRenderer::FixedUpdate()
{
}

void CMeshRenderer::SetMesh(const std::wstring& name)
{
	m_mesh = INSTANCE(CResourceManager).Get<CMesh>(name);
}

void CMeshRenderer::SetMaterial(const std::wstring& name)
{
	m_material = INSTANCE(CResourceManager).Get<CMaterial>(name);
}
