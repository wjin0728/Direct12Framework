#pragma once
#include"stdafx.h"

class CScene;
class CFrameResource;
class CGameObject;

class CSceneManager
{
	MAKE_SINGLETON(CSceneManager)

private:
	//게임에 쓰일 모든 씬들
	std::unordered_map<SCENE_TYPE, std::shared_ptr<CScene>> scenes{};
	//현재 씬
	SCENE_TYPE curSCENE_TYPE{};
	std::shared_ptr<CScene> curScene{};

	std::vector<std::shared_ptr<CGameObject>> maintainedObjects{};

public:

	void AddScene(SCENE_TYPE nextScene);
	void ChangeCurrentScene(SCENE_TYPE nextScene);

public:
	void InitCurrentScene();

	void ProcessInput(HWND hWnd);
	void Update();
	void Render();
	void ReleaseConstBuffer();
	void ChangeSceneViewport(int width, int height);

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void MaintainToAllScenes(const std::shared_ptr<CGameObject>& object) { maintainedObjects.push_back(object); }

	std::shared_ptr<CScene> GetCurScene() { return curScene; }
};

