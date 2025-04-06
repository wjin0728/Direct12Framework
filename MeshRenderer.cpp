#include "stdafx.h"
#include"DX12Manager.h"
#include "MeshRenderer.h"
#include"ResourceManager.h"
#include"Mesh.h"
#include"Material.h"
#include"Transform.h"
#include"Camera.h"
#include"Shader.h"
#include"InstancingBuffer.h"
#include"ObjectPoolManager.h"

CMeshRenderer::CMeshRenderer() : CComponent(COMPONENT_TYPE::MESH_RENDERER)
{
}


CMeshRenderer::~CMeshRenderer()
{
	ReturnCBVIndex();
}

void CMeshRenderer::Awake()
{
	SetCBVIndex();
}

void CMeshRenderer::Start()
{
}

void CMeshRenderer::Update()
{
}

void CMeshRenderer::LateUpdate()
{
	m_mesh->oobs.Transform(mWorldBS, GetTransform()->GetWorldMat());
}

void CMeshRenderer::Render(std::shared_ptr<CCamera> camera, int pass)
{
	if (!m_materials[0]->GetShader((PASS_TYPE)pass)) return;
	if (camera && !camera->IsInFrustum(mWorldBS)) return;

	CBObjectData objDate;
	objDate.worldMAt = GetTransform()->mWorldMat.Transpose();
	objDate.invWorldMAt = objDate.worldMAt.Invert();
	objDate.textureMat = GetTransform()->mTextureMat.Transpose();

	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::OBJECT)->UpdateBuffer(mCbvOffset, &objDate);
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::OBJECT)->BindToShader(mCbvOffset);

	int subMeshNum = m_mesh->GetSubMeshNum();
	for (int i = 0; i < subMeshNum; i++) {
		m_materials[i]->BindShader((PASS_TYPE)pass);
		m_materials[i]->BindDataToShader();
		m_mesh->Render(CMDLIST, i);
	}
}

void CMeshRenderer::InstancingRender(D3D12_VERTEX_BUFFER_VIEW ibv, UINT instancingNum)
{
	if (m_mesh) {
		m_mesh->Render(ibv, instancingNum, 0);
	}
}

void CMeshRenderer::SetCBVIndex()
{
	if (mCbvIdx < 0) {
		mCbvIdx = INSTANCE(CObjectPoolManager).GetTopCBVIndex();
		mCbvOffset = ALIGNED_SIZE(sizeof(CBObjectData)) * mCbvIdx;
	}
}

void CMeshRenderer::ReturnCBVIndex()
{
	if (mCbvIdx < 0) {
		return;
	}
	INSTANCE(CObjectPoolManager).ReturnCBVIndex(mCbvIdx);
	mCbvIdx = -1;
	mCbvOffset = 0;
}

void CMeshRenderer::SetMesh(const std::shared_ptr<CMesh>& mesh)
{ 
	m_mesh = mesh;
	if(m_mesh) mWorldBS = m_mesh->oobs;
}

void CMeshRenderer::SetMesh(const std::string& name)
{
	m_mesh = INSTANCE(CResourceManager).Get<CMesh>(name);
	if (m_mesh) mWorldBS = m_mesh->oobs;
}

void CMeshRenderer::AddMaterial(const std::shared_ptr<CMaterial>& material)
{
	m_materials.push_back(material);
}

void CMeshRenderer::AddMaterial(const std::string& name)
{
	m_materials.push_back(INSTANCE(CResourceManager).Get<CMaterial>(name));
}