#include "stdafx.h"
#include "MainScene.h"
#include"DX12Manager.h"
#include"InputManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include"ResourceManager.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"Transform.h"
#include"Camera.h"
#include"ServerManager.h"
#include"LoadingScreen.h"
#include"RenderManager.h"

CMainScene::CMainScene()
{
}

void CMainScene::Initialize()
{
}

void CMainScene::Start()
{
}

void CMainScene::Update()
{
	
	CScene::Update();
}

void CMainScene::LateUpdate()
{
	CScene::LateUpdate();
}

void CMainScene::RenderScene()
{
	auto finalPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	finalPassBuffer->BindToShader(0);
	auto renderTarget = RT_GROUP(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN);
	UINT backBufferIdx = INSTANCE(CDX12Manager).GetCurrBackBufferIdx();
	renderTarget->ChangeResourceToTarget(backBufferIdx);
	renderTarget->SetRenderTarget(backBufferIdx);
	renderTarget->ClearRenderTarget(backBufferIdx);

	Vec2 rtSize = INSTANCE(CDX12Manager).GetRenderTargetSize();

	D3D12_VIEWPORT mViewport = { 0.f,0.f,rtSize.x, rtSize.y };
	D3D12_RECT mScissorRect = { 0.f,0.f,rtSize.x, rtSize.y };

	CMDLIST->RSSetViewports(1, &mViewport);
	CMDLIST->RSSetScissorRects(1, &mScissorRect);
	mRenderMgr->RenderLayer(FORWARD, RENDER_LAYER::UI, nullptr);

	renderTarget->ChangeTargetToResource(backBufferIdx);
}
