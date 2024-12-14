#include "stdafx.h"
#include"DX12Manager.h"
#include "MeshRenderer.h"
#include"ResourceManager.h"
#include"Mesh.h"
#include"Material.h"
#include"Transform.h"
#include"Camera.h"
#include"InstancingBuffer.h"

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
	CBObjectData objDate;
	objDate.worldMAt = GetTransform()->mWorldMat.Transpose();
	objDate.invWorldMAt = objDate.worldMAt.Invert();
	objDate.textureMat = GetTransform()->mTextureMat.Transpose();

	int subMeshNum = m_mesh->GetSubMeshNum();
	for (int i = 0; i < subMeshNum; i++) {
		objDate.materialIdx = m_materials[i]->mSrvIdx;

		UPLOADBUFFER(CONSTANT_BUFFER_TYPE::OBJECT)->CopyData(&objDate, GetTransform()->mCbvIdx);
		UPLOADBUFFER(CONSTANT_BUFFER_TYPE::OBJECT)->UpdateBuffer(GetTransform()->mCbvIdx);

		m_mesh->Render(CMDLIST, i);
	}
}

void CMeshRenderer::InstancingRender(D3D12_VERTEX_BUFFER_VIEW ibv, UINT instancingNum)
{
	if (m_mesh) {
		m_mesh->Render(ibv, instancingNum, 0);
	}
}

void CMeshRenderer::SetMesh(const std::shared_ptr<CMesh>& mesh)
{ 
	m_mesh = mesh;
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
