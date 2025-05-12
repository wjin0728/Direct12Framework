#include "stdafx.h"
#include "GameApplication.h"
#include "Timer.h"
#include "DX12Manager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "ObjectPoolManager.h"
#include "ServerManager.h"
#include "Light.h"

bool CGameApplication::Initialize(HINSTANCE hInstance, WNDPROC wndProc, int cmdShow)
{
	mHInstance = hInstance;

	DEVMODE devMode = {};
	devMode.dmSize = sizeof(DEVMODE);
	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode))
	{
		clientWidth = devMode.dmPelsWidth;
		clientHeight = devMode.dmPelsHeight + 200;
	}


	//clientWidth = FRAMEBUFFER_WIDTH;
	//clientHeight = FRAMEBUFFER_HEIGHT;
	
	//윈도우 초기화
	if (!InitWindow(wndProc, cmdShow)) {
		return false;
	}
	SetWindowLongPtr(mHwnd, GWL_STYLE, WS_POPUP);
	SetWindowPos(mHwnd, HWND_TOP, 0, 0, clientWidth, clientHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	//매니저 초기화
	INSTANCE(CResourceManager).Initialize();
	INSTANCE(CObjectPoolManager).Initialize();
	INSTANCE(CDX12Manager).Initialize(mHwnd);
	INSTANCE(ServerManager).Initialize();
	TIMER.Initilaize();
	INPUT.Initialize(mHwnd);

	RESOURCE.LoadDefaultTexture();
	INSTANCE(ServerManager).Connect();
	INSTANCE(CSceneManager).LoadScene(SCENE_TYPE::LOADING);

	TIMER.Reset();

	return true;
}

int CGameApplication::Run()
{
	MSG msg{};

	auto& timer = TIMER;
	timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			INSTANCE(ServerManager).Recv_Packet();
			SleepEx(0, true);
			timer.Tick(60.f);
			Update();
			Render();
			ShowFPS();
		}
	}

	INSTANCE(CSceneManager).Destroy();
	INSTANCE(CResourceManager).Destroy();
	INSTANCE(CDX12Manager).Destroy();

	return (int)msg.wParam;
}


void CGameApplication::Update()
{
	INPUT.Update();
	INSTANCE(CDX12Manager).MoveToNextFrameResource();
	INSTANCE(CSceneManager).Update();
}

void CGameApplication::Render()
{
	INSTANCE(CDX12Manager).BeforeRender();
	INSTANCE(CSceneManager).Render();
	INSTANCE(CDX12Manager).AfterRender();
}

void CGameApplication::ShowFPS()
{
	auto frameRate = appName + TIMER.GetFrameRate();
	SetWindowText(mHwnd, frameRate.c_str());
}


void CGameApplication::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameApplication::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			break;
		case VK_SPACE:
			break;
		case VK_F9:
			INSTANCE(CDX12Manager).ChangeSwapChainState();
			return;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}


LRESULT CALLBACK CGameApplication::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		//if (LOWORD(wParam) == WA_INACTIVE)
		//	INSTANCE(CGameTimer).Stop();
		//else
		//	INSTANCE(CGameTimer).Start();
		INSTANCE(CGameTimer).Start();
		break;
	}
	case WM_SIZE:
		clientWidth = LOWORD(lParam);
		clientHeight = HIWORD(lParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, nMessageID, wParam, lParam);;
}


bool CGameApplication::InitWindow(WNDPROC wndProc, int cmdShow)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mHInstance;
	wcex.hIcon = LoadIcon(mHInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MainWindow";
	wcex.hIconSm = ::LoadIcon(wcex.hInstance, IDI_APPLICATION);
	
	if (!RegisterClassExW(&wcex))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT rc = { 0, 0, clientWidth, clientHeight };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;

	AdjustWindowRect(&rc, dwStyle, false);

	mHwnd = CreateWindow(L"MainWindow", appName.c_str(), dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, mHInstance, nullptr);

	if (!mHwnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mHwnd, SW_SHOWMAXIMIZED);
	UpdateWindow(mHwnd);
	SetForegroundWindow(mHwnd);
	SetFocus(mHwnd);

	return true;
}



