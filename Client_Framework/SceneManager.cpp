#include "stdafx.h"
#include "SceneManager.h"
#include"DX12Manager.h"
#include "Scene.h"
#include"LobbyScene.h"
#include"BattleScene.h"
#include"MenuScene.h"
#include"LoadScene.h"
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
	switch (nextScene)
	{
	case SCENE_TYPE::LOBBY:
		curScene = std::make_shared<CLobbyScene>();
		break;
	case SCENE_TYPE::LOADING:
		curScene = std::make_shared<CLoadScene>();
		break;
	case SCENE_TYPE::MENU:
		curScene = std::make_shared<CMenuScene>();
		break;
	case SCENE_TYPE::MAINSTAGE1:
		curScene = std::make_shared<CBattleScene>();
		break;
	case SCENE_TYPE::MAINSTAGE2:
		curScene = std::make_shared<CBattle2Scene>();
		break;
	case SCENE_TYPE::END:
		break;
	default:
		break;
	}
	curSceneType = nextScene;
	curScene->Initialize();
	curScene->Awake();
	curScene->Start();
	curScene->mIsActive = true;
	INSTANCE(CResourceManager).ProcessGPULoadQueue();
}

void CSceneManager::ChangeScene(SCENE_TYPE nextScene, bool savePrevScene)
{
	if (savePrevScene) {
		if (curScene) {
			prevScene = curScene;
			curScene->mIsActive = false;
		}
	}
	else {
		INSTANCE(CInstancingManager).Destroy();
		curScene.reset();
	}
	LoadScene(nextScene);
}

void CSceneManager::ChangeScene(SceneChangeReq req)
{
	if (req.savePrevScene) {
		if (curScene) {
			prevScene = curScene;
		}
	}
	else {
		INSTANCE(CInstancingManager).Destroy();
		curScene.reset();
	}
	LoadScene(req.changeScene);
}

void CSceneManager::ReturnPrevScene()
{
	if (!prevScene) {
		return;
	}
	curScene = prevScene;
}

void CSceneManager::RequestSceneChange(SCENE_TYPE nextScene, bool savePrevScene)
{
	SceneChangeReq req(nextScene, savePrevScene);
	sceneChangeQueue.push(req);
}

void CSceneManager::ProcessSceneChangeQueue()
{
	if (sceneChangeQueue.empty()) {
		return;
	}
	SceneChangeReq req = sceneChangeQueue.front();
	sceneChangeQueue.pop();
	ChangeScene(req);
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