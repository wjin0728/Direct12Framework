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
#include"UploadBuffer.h"
#include "ObjectPoolManager.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include"Transform.h"

std::shared_ptr<class CMesh> CUIRenderer::mQuad = nullptr;

CUIRenderer::CUIRenderer()
{
	type = COMPONENT_TYPE::RENDERER;
	mUIData.type = 0;
	mUIData.color = Vec4(1.f, 1.f, 1.f, 1.f);
	mUIData.pos = Vec2(0.f, 0.f);
	mUIData.size = Vec2(1.f, 1.f);
	mUIData.uvOffset = Vec2(0.f, 0.f);
	mUIData.uvScale = Vec2(1.f, 1.f);
}

CUIRenderer::~CUIRenderer()
{
	ReturnUICBVIndex();
	ReturnCBVIndex();
}

void CUIRenderer::Awake()
{
}

void CUIRenderer::Start()
{
	SetUICBVIndex();
	SetCBVIndex();

	mDirtyFrame = mUIDirtyFrame = FRAME_RESOURCE_COUNT + 1;
	UpdataObjectDataToShader();
	UpdateUIDataToShader();
}

void CUIRenderer::Update()
{
}

void CUIRenderer::LateUpdate()
{
}

void CUIRenderer::Render(std::shared_ptr<CCamera> camera, int pass)
{
	mShader->SetPipelineState(CMDLIST);

	UpdataObjectDataToShader();
	auto objectBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::OBJECT);
	auto UIDataBuffer = STRUCTEDBUFFER((UINT)STRUCTED_BUFFER_TYPE::UI);

	objectBuffer->BindToShader(mCbvOffset);
	UpdateUIDataToShader();
	mQuad->Render(CMDLIST);
}

void CUIRenderer::UpdataObjectDataToShader()
{
	if (mDirtyFrame > 0) {
		CBObjectData objDate;
		objDate.worldMAt = GetTransform()->mWorldMat.Transpose();
		objDate.invWorldMAt = GetTransform()->mWorldMat.Invert();
		objDate.textureMat = GetTransform()->mTextureMat.Transpose();
		objDate.idx0 = mUIdataIdx;

		auto objectBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::OBJECT);
		objectBuffer->UpdateBuffer(mCbvOffset, &objDate);
		mDirtyFrame--;
	}
}

void CUIRenderer::UpdateUIDataToShader()
{
	if (mUIDirtyFrame > 0) {
		auto UIDataBuffer = STRUCTEDBUFFER((UINT)STRUCTED_BUFFER_TYPE::UI);
		UIDataBuffer->UpdateBuffer(mUIdataIdx, &mUIData);
		mUIDirtyFrame--;
	}
}

void CUIRenderer::SetUICBVIndex()
{
	if (mUIdataIdx < 0) {
		mUIdataIdx = INSTANCE(CObjectPoolManager).GetUICBVIndex();
	}
}

void CUIRenderer::ReturnUICBVIndex()
{
	if (mUIdataIdx < 0) {
		return;
	}
	INSTANCE(CObjectPoolManager).ReturnUICBVIndex(mUIdataIdx);
	mUIdataIdx = -1;
}

void CUIRenderer::SetShader(const std::string& name)
{
	mShader = RESOURCE.Get<CShader>(name); 
	mUIDirtyFrame = FRAME_RESOURCE_COUNT;
}

void CUIRenderer::SetTexture(const std::string& name)
{
	auto texture = RESOURCE.Get<CTexture>(name);
	if (texture) {
		mUIData.textureIdx = texture->GetSrvIndex();
		mUIDirtyFrame = FRAME_RESOURCE_COUNT;
	}
}

void CUIRenderer::SetTexture(const std::shared_ptr<class CTexture>& texture)
{
	if (!texture) {
		return;
	}
	mUIData.textureIdx = texture->GetSrvIndex(); mUIDirtyFrame = FRAME_RESOURCE_COUNT;
}
