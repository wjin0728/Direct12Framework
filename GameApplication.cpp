#include "stdafx.h"
#include "GameApplication.h"
#include"Timer.h"
#include"DX12Manager.h"
#include"ResourceManager.h"


CGameApplication::~CGameApplication()
{
	INSTANCE(CDX12Manager).Destroy();
}

bool CGameApplication::Initialize(HINSTANCE hInstance, WNDPROC wndProc, int cmdShow)
{
	m_hInstance = hInstance;
	clientWidth = FRAMEBUFFER_WIDTH;
	clientHeight = FRAMEBUFFER_HEIGHT;
	
	//윈도우 초기화
	if (!InitWindow(wndProc, cmdShow)) {
		return false;
	}

	//매니저 초기화
	INSTANCE(CDX12Manager).Initialize(m_hWnd);
	INSTANCE(CResourceManager).Initialize();
	INSTANCE(CGameTimer).Initilaize();

	
	//메인 씬 초기화
	INSTANCE(CDX12Manager).OpenCommandList();
	sceneStateMachine.AddScene(SCENE_TYPE::MENU);
	sceneStateMachine.InitCurrentScene();
	INSTANCE(CDX12Manager).CloseCommandList();

	sceneStateMachine.ReleaseConstBuffer();

	INSTANCE(CGameTimer).Reset();

	return true;
}

int CGameApplication::Run()
{
	MSG msg{};

	auto& timer = INSTANCE(CGameTimer);
	timer.Reset();

	while (1)
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

	INSTANCE(CDX12Manager).Destroy();

	return (int)msg.wParam;
}


void CGameApplication::ProcessInput()
{
	sceneStateMachine.ProcessInput(m_hWnd);
}

void CGameApplication::Update()
{
	INSTANCE(CDX12Manager).MoveToNextFrameResource();

	sceneStateMachine.Update();
}

void CGameApplication::Render()
{
	INSTANCE(CDX12Manager).BeforeRender();
	sceneStateMachine.Render();
	INSTANCE(CDX12Manager).AfterRender();
}

void CGameApplication::ShowFPS()
{
	auto frameRate = appName + INSTANCE(CGameTimer).GetFrameRate();
	SetWindowText(m_hWnd, frameRate.c_str());
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

	if (sceneStateMachine.OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam)) {
		INSTANCE(CDX12Manager).OpenCommandList();

		sceneStateMachine.InitCurrentScene();

		INSTANCE(CDX12Manager).CloseCommandList();

		sceneStateMachine.ReleaseConstBuffer();

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
			sceneStateMachine.ChangeSceneViewport(clientWidth, clientHeight);
			return;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	if (sceneStateMachine.OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam)) {
		INSTANCE(CDX12Manager).OpenCommandList();

		sceneStateMachine.InitCurrentScene();

		INSTANCE(CDX12Manager).CloseCommandList();

		sceneStateMachine.ReleaseConstBuffer();

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
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION);
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

	HWND hMainWnd = CreateWindow(L"MainWindow", appName.c_str(), dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, 
		rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, m_hInstance, nullptr);

	if (!hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(hMainWnd, cmdShow);
	UpdateWindow(hMainWnd);

	return true;
}



