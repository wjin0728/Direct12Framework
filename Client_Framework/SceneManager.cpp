#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"
#include"DX12Manager.h"
#include"LobbyScene.h"
#include"BattleScene.h"
#include"MainScene.h"
#include"LoadScene.h"
#include"ResourceManager.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"RenderManager.h"
#include"ParticleManager.h"

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
		curScene = std::make_shared<CMainScene>();
		break;
	case SCENE_TYPE::MAINSTAGE1:
		curScene = std::make_shared<CBattleScene>();
		break;
	case SCENE_TYPE::MAINSTAGE2:
		curScene = std::make_shared<CBattle2Scene>();
		break;
	case SCENE_TYPE::MAINSTAGE3:
		curScene = std::make_shared<CBattle3Scene>();
		break;
	case SCENE_TYPE::END:
		break;
	default:
		break;
	}
	curSceneType = nextScene;
	curScene->Initialize();
	curScene->Activate();
	curScene->Start();
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
		INSTANCE(CRenderManager).Destroy();
		INSTANCE(CParticleManager).ReleaseAllParticleEmitters();
		if (curScene) {
			curScene->mIsActive = false;
			curScene.reset();
		}
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
		INSTANCE(CRenderManager).Destroy();
		INSTANCE(CParticleManager).ReleaseAllParticleEmitters();
		if (curScene) {
			curScene->mIsActive = false;
			curScene.reset();

		}
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
	curScene->CommitObjectChanges();
	curScene->Update();
	curScene->LateUpdate();
}

void CSceneManager::Render()
{
	if (!curScene) {
		return;
	}
	curScene->CollectVisibleObjects();
	curScene->RenderScene();
	curScene->RenderFadeOverlay();
}

bool CSceneManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CSceneManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}