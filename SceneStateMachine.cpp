#include "stdafx.h"
#include "Scene.h"
#include "SceneStateMachine.h"

CSceneStateMachine::~CSceneStateMachine()
{
}

void CSceneStateMachine::AddScene(SCENE_TYPE nextScene)
{
	if (scenes.find(nextScene) != scenes.end()) {
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

		curSCENE_TYPE = nextScene;
		curScene = scenes[curSCENE_TYPE].get();
	}
}

void CSceneStateMachine::ChangeCurrentScene(SCENE_TYPE nextScene)
{
	if (scenes.find(nextScene) != scenes.end()) {
		if (scenes[nextScene]) 
		{
			curScene->Destroy();
			curSCENE_TYPE = nextScene;
			curScene = scenes[curSCENE_TYPE].get();
		}
	}
}

void CSceneStateMachine::InitCurrentScene()
{
	curScene->Initialize();
}

void CSceneStateMachine::ProcessInput(HWND hWnd)
{
	curScene->ProcessInput(hWnd);
}

void CSceneStateMachine::Update()
{
	curScene->Update();
}

void CSceneStateMachine::Render()
{
	curScene->Render();
}

void CSceneStateMachine::ReleaseConstBuffer()
{
}

void CSceneStateMachine::ChangeSceneViewport(int width, int height)
{
	curScene->ChangeViewport(width, height);
}


bool CSceneStateMachine::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return curScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
}

bool CSceneStateMachine::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return curScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
}
