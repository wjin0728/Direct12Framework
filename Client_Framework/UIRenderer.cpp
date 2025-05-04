#include "stdafx.h"
#include "UIRenderer.h"
#include "DX12Manager.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Material.h"
#include"Mesh.h"
#include"FrameResource.h"	

std::shared_ptr<class CMesh> CUIRenderer::mQuad = nullptr;

CUIRenderer::CUIRenderer()
{
}

CUIRenderer::~CUIRenderer()
{
}

void CUIRenderer::Render(std::shared_ptr<CCamera> camera, int pass)
{
	mShader->SetPipelineState(CMDLIST);
}

void CUIRenderer::SetTexture(const std::string& name)
{
	auto texture = RESOURCE.Get<CTexture>(name);
	if (texture) {
		mUIData.textureIdx = texture->GetSrvIndex();
	}
	else {
		mUIData.textureIdx = -1;
	}
}
