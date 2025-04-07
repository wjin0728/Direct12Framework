#include "stdafx.h"
#include "Renderer.h"
#include "ResourceManager.h"

CRenderer::CRenderer() : CComponent(COMPONENT_TYPE::RENDERER)
{
}

CRenderer::~CRenderer() 
{
}

void CRenderer::AddMaterial(const std::shared_ptr<CMaterial>& material)
{
	m_materials.push_back(material);
}

void CRenderer::AddMaterial(const std::string& name)
{
	m_materials.push_back(INSTANCE(CResourceManager).Get<CMaterial>(name));
}