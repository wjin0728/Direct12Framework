#include "stdafx.h"
#include "SceneManager.h"
#include"DX12Manager.h"
#include "Scene.h"
#include"MainScene.h"
#include"BattleScene.h"
#include"MenuScene.h"
#include"ResourceManager.h"
#include"GameObject.h"
#include"MeshRenderer.h"

//#define DEFFERD_RENDERING


void CSceneManager::Destroy()
{
	curScene.reset();
	prevScene.reset();
}

void CSceneManager::LoadScene(SCENE_TYPE nextScene)
{
	INSTANCE(CDX12Manager).OpenCommandList();

	switch (nextScene)
	{
	case SCENE_TYPE::MAIN:
		curScene = std::make_shared<CMainScene>();
		break;
	case SCENE_TYPE::MENU:
		curScene = std::make_shared<CMenuScene>();
		break;
	case SCENE_TYPE::MAINSTAGE:
		curScene = std::make_shared<CBattleScene>();
		break;
	case SCENE_TYPE::END:
		break;
	default:
		break;
	}
	curScene->Initialize();
	curScene->Awake();
	curScene->Start();

	INSTANCE(CDX12Manager).CloseCommandList();
	INSTANCE(CResourceManager).ReleaseUploadBuffers();
}

void CSceneManager::ChangeScene(SCENE_TYPE nextScene)
{
	prevScene = curScene;
	LoadScene(nextScene);
}

void CSceneManager::ReturnPrevScene()
{
	if (!prevScene) {
		return;
	}
	curScene = prevScene;
}

void CSceneManager::InitCurrentScene()
{
}

void CSceneManager::Update()
{
	if (!curScene) {
		return;
	}
	curScene->Update();
	curScene->LateUpdate();
}

void CSceneManager::Render()
{
	if (!curScene) {
		return;
	}
	curScene->RenderScene();
}

bool CSceneManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CSceneManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}