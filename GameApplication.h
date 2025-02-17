#pragma once

#include "Scene.h"
#include "Timer.h"

class CGameApplication
{
public:
	CGameApplication() {};
	~CGameApplication() {};

private:
	HINSTANCE mHInstance = nullptr;
	HWND mHwnd = nullptr;

	std::wstring appName = L"졸작 (";

	bool active = true;

	int clientWidth;
	int clientHeight;

private:
	//게임 매니저들
	/*class CDX12Manager& DX12Mgr;
	class CGameTimer& Timer;
	class CSceneManager& sceneMgr;*/

public:
	bool Initialize(HINSTANCE hInstance, WNDPROC wndProc, int cmdShow);

	int Run();

	void Update();
	void Render();
	void ShowFPS();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	bool InitWindow(WNDPROC wndProc, int cmdShow);

public:
	HWND GetHWnd() const { return mHwnd; }
};

