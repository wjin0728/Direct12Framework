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

CMeshRenderer::CMeshRenderer() : CRenderer()
{
	type = COMPONENT_TYPE::MESH_RENDERER; // 타입 재설정
}

CMeshRenderer::~CMeshRenderer()
{
}

void CMeshRenderer::Awake()
{
	CRenderer::Awake();
}

void CMeshRenderer::Start()
{
	CRenderer::Start();
	UpdataObjectDataToShader();
}

void CMeshRenderer::Update()
{
}

void CMeshRenderer::LateUpdate()
{
	if (!m_mesh) return;
	m_mesh->oobs.Transform(mWorldBS, GetTransform()->GetWorldMat());
	m_mesh->oobb.Transform(mWorldOOBB, GetTransform()->GetWorldMat());
	UpdataObjectDataToShader();
}

void CMeshRenderer::Render(std::shared_ptr<CCamera> camera, int pass)
{
	if (!m_mesh) return;
	if (!m_materials[0]) return;
	if (!m_materials[0]->GetShader((PASS_TYPE)pass)) return;
	if (camera && !camera->IsInFrustum(mWorldBS)) return;

	auto objectBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::OBJECT);
	
	objectBuffer->BindToShader(mCbvOffset);

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

