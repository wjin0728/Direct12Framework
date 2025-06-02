#pragma once
#include"stdafx.h"

class CScene;
class CFrameResource;
class CGameObject;

#define CUR_SCENE INSTANCE(CSceneManager).GetCurScene()

struct SceneChangeReq
{
	SCENE_TYPE changeScene{ SCENE_TYPE::END };
	bool savePrevScene{ false };
	SceneChangeReq(SCENE_TYPE scene, bool save) : changeScene(scene), savePrevScene(save) {}
	SceneChangeReq() = default;
	SceneChangeReq(const SceneChangeReq& other) : changeScene(other.changeScene), savePrevScene(other.savePrevScene) {}
	~SceneChangeReq() = default;
};

class CSceneManager
{
	MAKE_SINGLETON(CSceneManager)

private:
	std::shared_ptr<CScene> curScene{ nullptr };
	std::shared_ptr<CScene> prevScene{ nullptr };
	SCENE_TYPE curSceneType{ SCENE_TYPE::END };

	std::vector<std::shared_ptr<CGameObject>> maintainedObjects{};
	std::queue<SceneChangeReq> sceneChangeQueue{};


public:
	void Destroy();

	void LoadScene(SCENE_TYPE nextScene);
	void ChangeScene(SCENE_TYPE nextScene, bool savePrevScene = false);
	void ChangeScene(SceneChangeReq req);
	void ReturnPrevScene();

	void RequestSceneChange(SCENE_TYPE nextScene, bool savePrevScene = false);
	void ProcessSceneChangeQueue();

public:
	void InitCurrentScene();

	void Update();
	void Render();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void MaintainToAllScenes(const std::shared_ptr<CGameObject>& object) { maintainedObjects.push_back(object); }

	std::shared_ptr<CScene> GetCurScene() { return curScene; }
	SCENE_TYPE GetCurSceneType() const { return curSceneType; }

};

