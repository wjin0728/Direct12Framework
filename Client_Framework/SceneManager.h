#pragma once
#include"stdafx.h"

class CScene;
class CFrameResource;
class CGameObject;

#define CUR_SCENE INSTANCE(CSceneManager).GetCurScene()

class CSceneManager
{
	MAKE_SINGLETON(CSceneManager)

private:
	std::shared_ptr<CScene> curScene{ nullptr };
	std::shared_ptr<CScene> prevScene{ nullptr };

	std::vector<std::shared_ptr<CGameObject>> maintainedObjects{};

public:
	void Destroy();

	void LoadScene(SCENE_TYPE nextScene);
	void ChangeScene(SCENE_TYPE nextScene, bool savePrevScene = false);
	void ReturnPrevScene();

public:
	void InitCurrentScene();

	void Update();
	void Render();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void MaintainToAllScenes(const std::shared_ptr<CGameObject>& object) { maintainedObjects.push_back(object); }

	std::shared_ptr<CScene> GetCurScene() { return curScene; }

};

