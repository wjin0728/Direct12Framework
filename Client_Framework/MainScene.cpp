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
#include "MainMenuUI.h"

CMainScene::CMainScene()
{
}

void CMainScene::Initialize()
{
	auto mainUI = RESOURCE.GetPrefab("StartUI");
	if (mainUI) {
		auto uiObject = CGameObject::Instantiate(mainUI);
		uiObject->SetName("StartUI");
		uiObject->AddComponent<CMainMenu>();
		AddObjectImmediately(uiObject);
	}
	INPUT.FixMousePosition(false);
	FadeOut(1.f, { 0.f, 0.f, 0.f, 1.f });
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
	mRenderMgr->RenderUIPass();
}
