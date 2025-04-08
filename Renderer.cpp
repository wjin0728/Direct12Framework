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