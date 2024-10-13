#pragma once
#include"stdafx.h"
#include"Shader.h"
#include"LightManager.h"
#include"Texture.h"
#include"FrameResource.h"

class CCamera;
class CGameObject;
class CTerrain;
class CPlayer;

class CSceneManager;

class CScene
{
protected:
	using ObjectList = std::vector<std::shared_ptr<CGameObject>>;

	CSceneManager& sceneStateMachine;

	std::unique_ptr<CLightManager> lightManager{};

public:
	CScene(CSceneManager& sceneStateMachine);
	virtual ~CScene() = default;

	virtual void Initialize() = 0;
	virtual void Destroy() = 0;

	virtual void InitLights() = 0;
	virtual void BuildObjects() = 0;

	virtual void ReleaseObjects() = 0;

	virtual void ProcessInput(HWND hWnd) = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;

	virtual void ChangeViewport(int width, int height) = 0;
};


class CMenuScene : public CScene
{
public:
	CMenuScene(CSceneManager& sceneStateMachine);
	virtual ~CMenuScene() = default;

private:
	std::unordered_map<std::wstring, ObjectList> objects;

	std::shared_ptr<CCamera> camera;

public:
	virtual void Initialize() override;
	virtual void Destroy() override;

	virtual void InitLights();
	virtual void BuildObjects() override;

	virtual void ReleaseObjects() override;

	virtual void ProcessInput(HWND hWnd) override;
	virtual void Update() override;
	virtual void Render() override;

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	virtual void ChangeViewport(int width, int height) override;

};


class CPlayScene : public CScene
{
public:
	CPlayScene(CSceneManager& sceneStateMachine);
	virtual ~CPlayScene() = default;

private:
	std::unordered_map<std::wstring, ObjectList> objects;

	std::shared_ptr<CCamera> camera;

	std::shared_ptr<CPlayer> m_pPlayer;

	std::shared_ptr<CTerrain> terrain;

	std::shared_ptr<CGameObject> lockedObject;

	POINT m_ptOldCursorPos;

public:
	virtual void Initialize() override;
	virtual void Destroy() override;

	virtual void InitLights();
	virtual void BuildObjects() override;

	virtual void ReleaseObjects() override;

	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision();
	void CheckObjectByBulletCollisions();
	void CheckPlayerByBulletCollision();

	virtual void ProcessInput(HWND hWnd) override;
	virtual void Update() override;
	virtual void Render() override;

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	virtual void ChangeViewport(int width, int height) override;

	std::shared_ptr<CGameObject> PickObjectPointedByCursor(int xClient, int yClient);

};

