#include "stdafx.h"
#include "GameApplication.h"
#include"Timer.h"
#include"DX12Manager.h"
#include"ResourceManager.h"
#include"SceneStateMachine.h"



bool CGameApplication::Initialize(HINSTANCE hInstance, WNDPROC wndProc, int cmdShow)
{
	mHInstance = hInstance;
	clientWidth = FRAMEBUFFER_WIDTH;
	clientHeight = FRAMEBUFFER_HEIGHT;
	
	//윈도우 초기화
	if (!InitWindow(wndProc, cmdShow)) {
		return false;
	}

	//매니저 초기화
	INSTANCE(CDX12Manager).Initialize(mHwnd);

	INSTANCE(CDX12Manager).OpenCommandList();

	INSTANCE(CResourceManager).Initialize();
	INSTANCE(CGameTimer).Initilaize();

	//메인 씬 초기화
	INSTANCE(CSceneManager).AddScene(SCENE_TYPE::MENU);
	INSTANCE(CSceneManager).InitCurrentScene();
	INSTANCE(CDX12Manager).CloseCommandList();

	INSTANCE(CSceneManager).ReleaseConstBuffer();

	INSTANCE(CGameTimer).Reset();

	return true;
}

int CGameApplication::Run()
{
	MSG msg{};

	auto& timer = INSTANCE(CGameTimer);
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
			timer.Tick(60.0f);

			ProcessInput();
			Update();
			Render();

			ShowFPS();
		}
	}

	//INSTANCE(CDX12Manager).Destroy();

	return (int)msg.wParam;
}


void CGameApplication::ProcessInput()
{
	INSTANCE(CSceneManager).ProcessInput(mHwnd);
}

void CGameApplication::Update()
{
	INSTANCE(CDX12Manager).MoveToNextFrameResource();

	INSTANCE(CSceneManager).Update();
}

void CGameApplication::Render()
{
	INSTANCE(CDX12Manager).BeforeRender();
	//sceneStateMachine.Render();
	INSTANCE(CDX12Manager).AfterRender();
}

void CGameApplication::ShowFPS()
{
	auto frameRate = appName + INSTANCE(CGameTimer).GetFrameRate();
	SetWindowText(mHwnd, frameRate.c_str());
}


void CGameApplication::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}

	if (INSTANCE(CSceneManager).OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam)) {
		INSTANCE(CDX12Manager).OpenCommandList();

		INSTANCE(CSceneManager).InitCurrentScene();

		INSTANCE(CDX12Manager).CloseCommandList();

		INSTANCE(CSceneManager).ReleaseConstBuffer();

		INSTANCE(CGameTimer).Reset();
	}
}

void CGameApplication::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			return;
			break;
		case VK_RETURN:
			break;
		case VK_SPACE:
			break;
		case VK_F9:
			INSTANCE(CDX12Manager).ChangeSwapChainState();
			INSTANCE(CSceneManager).ChangeSceneViewport(clientWidth, clientHeight);
			return;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	if (INSTANCE(CSceneManager).OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam)) {
		INSTANCE(CDX12Manager).OpenCommandList();

		INSTANCE(CSceneManager).InitCurrentScene();

		INSTANCE(CDX12Manager).CloseCommandList();

		INSTANCE(CSceneManager).ReleaseConstBuffer();

		INSTANCE(CGameTimer).Reset();
	}
}


LRESULT CALLBACK CGameApplication::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			INSTANCE(CGameTimer).Stop();
		else
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

	ShowWindow(mHwnd, cmdShow);
	UpdateWindow(mHwnd);

	return true;
}



