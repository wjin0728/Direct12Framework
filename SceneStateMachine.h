#pragma once
#include"stdafx.h"

class CScene;
class CFrameResource;

class CSceneStateMachine
{
private:
	//게임에 쓰일 모든 씬들
	std::unordered_map<SCENE_TYPE, std::unique_ptr<CScene>> scenes;
	//현재 씬
	SCENE_TYPE curSCENE_TYPE;
	CScene* curScene;

public:
	~CSceneStateMachine();

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
};

