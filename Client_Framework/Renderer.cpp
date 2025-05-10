#include "stdafx.h"
#include"DX12Manager.h"
#include"ResourceManager.h"
#include"Material.h"
#include"Transform.h"
#include"Camera.h"
#include"Shader.h"
#include"InstancingBuffer.h"
#include"ObjectPoolManager.h"
#include"Renderer.h"

CRenderer::CRenderer() : CComponent(COMPONENT_TYPE::RENDERER)
{
}

CRenderer::~CRenderer() 
{
	ReturnCBVIndex();
}

void CRenderer::Awake()
{
	SetCBVIndex();
}

void CRenderer::Start()
{
	
}

void CRenderer::UpdataObjectDataToShader()
{
	if (GetTransform()->mDirtyFlag) {
		mDirtyFrame = FRAME_RESOURCE_COUNT;
	}
	mDirtyFrame = FRAME_RESOURCE_COUNT;
	if(mDirtyFrame > 0) {
		CBObjectData objDate;
		objDate.worldMAt = GetTransform()->mWorldMat.Transpose();
		objDate.invWorldMAt = GetTransform()->mWorldMat.Invert();
		objDate.textureMat = GetTransform()->mTextureMat.Transpose();

		auto objectBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::OBJECT);
		objectBuffer->UpdateBuffer(mCbvOffset, &objDate);
		mDirtyFrame--;
	}
}

void CRenderer::RenderFullscreen()
{
	// Implementation for rendering a fullscreen quad
	CMDLIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		CMDLIST->DrawInstanced(3, 1, 0, 0);
}

void CRenderer::SetCBVIndex()
{
	if (mCbvIdx < 0) {
		mCbvIdx = INSTANCE(CObjectPoolManager).GetTopCBVIndex();
		mCbvOffset = ALIGNED_SIZE(sizeof(CBObjectData)) * mCbvIdx;
	}
}

void CRenderer::ReturnCBVIndex()
{
	if (mCbvIdx < 0) {
		return;
	}
	INSTANCE(CObjectPoolManager).ReturnCBVIndex(mCbvIdx);
	mCbvIdx = -1;
	mCbvOffset = 0;
}

void CRenderer::AddMaterial(const std::shared_ptr<CMaterial>& material)
{
	m_materials.push_back(material);
}

void CRenderer::AddMaterial(const std::string& name)
{
	m_materials.push_back(INSTANCE(CResourceManager).Get<CMaterial>(name));
}