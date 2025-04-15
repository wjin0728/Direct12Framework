#include "stdafx.h"
#include "GameApplication.h"
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")


CGameApplication gameApp;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	try 
	{
		if (!gameApp.Initialize(hInstance, WndProc, showCmd)) {
			return 0;
		}
		return gameApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(NULL, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
	
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return gameApp.OnProcessingWindowMessage(hWnd, message, wParam, lParam);;
}
