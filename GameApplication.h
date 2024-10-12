#pragma once

#include "Player.h"
#include "Scene.h"
#include "Timer.h"
#include"SceneStateMachine.h"

class CGameApplication
{
public:
	CGameApplication() = default;
	~CGameApplication();

private:
	HINSTANCE m_hInstance = nullptr;
	HWND m_hWnd = nullptr;

	std::wstring appName = L"¿ìÁö´Ï²¨ (";

	bool active = true;

	int clientWidth;
	int clientHeight;

	CSceneStateMachine sceneStateMachine;

public:
	bool Initialize(HINSTANCE hInstance, WNDPROC wndProc, int cmdShow);

	int Run();

	void ProcessInput();
	void Update();
	void Render();
	void ShowFPS();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	bool InitWindow(WNDPROC wndProc, int cmdShow);
};

