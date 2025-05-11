#include "stdafx.h"
#include "LoadScene.h"
#include"SceneManager.h"
#include"ResourceManager.h"
#include"DX12Manager.h"
#include"ServerManager.h"
#include"GameObject.h"
#include"Transform.h"
#include"LoadingScreen.h"


CLoadScene::CLoadScene()
	: CScene()
{
}

void CLoadScene::Initialize()
{
	//INSTANCE(ServerManager).RegisterPlayerInScene(this);
	RESOURCE.LoadLoadingScreen();

	Vec2 rtSize = INSTANCE(CDX12Manager).GetRenderTargetSize();
	auto loadingScreenUI = CGameObject::CreateUIObject("Sprite", "Loading0", {0.f,0.f}, rtSize);
	auto loadingScreenMoving = loadingScreenUI->AddComponent<CLoadingScreen>();
	loadingScreenMoving->SetLoadingScreenSpeed(0.2f);
	for (int i = 0; i < 16; i++) {
		std::string name = "Loading" + std::to_string(i);
		auto loadingScreen = RESOURCE.Get<CTexture>(name);

		loadingScreenMoving->AddLoadingScreenTextureIdx(loadingScreen->GetSrvIndex());
	}
	mLoadingScreen = loadingScreenMoving;
}

void CLoadScene::Awake()
{
	CScene::Awake();
}

void CLoadScene::Start()
{
	RESOURCE.RequestLoad();
	CScene::Start();
}

void CLoadScene::Update()
{
	auto& resourceMgr = RESOURCE;

	if (resourceMgr.IsLoadFinished())
	{
		resourceMgr.ProcessGPULoadQueue(3);
		if (resourceMgr.IsGPULoadQueueEmpty())
		{
			mLoadingScreen->SetIsLoading(false);
		}

	}
	CScene::Update();
}

void CLoadScene::LateUpdate()
{
	CScene::LateUpdate();
}

void CLoadScene::RenderScene()
{
	auto finalPassBuffer = CONSTANTBUFFER((UINT)CONSTANT_BUFFER_TYPE::PASS);
	finalPassBuffer->BindToShader(0);
	auto renderTarget = RT_GROUP(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN);
	UINT backBufferIdx = INSTANCE(CDX12Manager).GetCurrBackBufferIdx();
	renderTarget->ChangeResourceToTarget(backBufferIdx);
	renderTarget->SetRenderTarget(backBufferIdx);
	renderTarget->ClearRenderTarget(backBufferIdx);

	Vec2 rtSize = INSTANCE(CDX12Manager).GetRenderTargetSize();

	D3D12_VIEWPORT mViewport = {0.f,0.f,rtSize.x, rtSize.y};
	D3D12_RECT mScissorRect = { 0.f,0.f,rtSize.x, rtSize.y };

	CMDLIST->RSSetViewports(1, &mViewport);
	CMDLIST->RSSetScissorRects(1, &mScissorRect);
	RenderForLayer("UI", nullptr);

	renderTarget->ChangeTargetToResource(backBufferIdx);
}

