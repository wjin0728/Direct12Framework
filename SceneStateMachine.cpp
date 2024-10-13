#include "stdafx.h"
#include "Scene.h"
#include "SceneStateMachine.h"


void CSceneManager::AddScene(SCENE_TYPE nextScene)
{
	if (scenes.find(nextScene) == scenes.end()) {
		switch (nextScene)
		{
		case SCENE_TYPE::MENU:
			scenes.emplace(SCENE_TYPE::MENU, new CMenuScene(*this));
			break;
		case SCENE_TYPE::MAINSTAGE:
			scenes.emplace(SCENE_TYPE::MAINSTAGE, new CPlayScene(*this));
			break;
		default:
			break;
		}

		
	}
	curSCENE_TYPE = nextScene;
	curScene = scenes[curSCENE_TYPE];
}

void CSceneManager::ChangeCurrentScene(SCENE_TYPE nextScene)
{
	if (scenes.find(nextScene) != scenes.end()) {
		if (scenes[nextScene]) 
		{
			curScene->Destroy();
			curSCENE_TYPE = nextScene;
			curScene = scenes[curSCENE_TYPE];
		}
	}
}

void CSceneManager::InitCurrentScene()
{
	curScene->Initialize();
}

void CSceneManager::ProcessInput(HWND hWnd)
{
	curScene->ProcessInput(hWnd);
}

void CSceneManager::Update()
{
	curScene->Update();
}

void CSceneManager::Render()
{
	curScene->Render();
}

void CSceneManager::ReleaseConstBuffer()
{
}

void CSceneManager::ChangeSceneViewport(int width, int height)
{
	curScene->ChangeViewport(width, height);
}


bool CSceneManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return curScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
}

bool CSceneManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return curScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
}
