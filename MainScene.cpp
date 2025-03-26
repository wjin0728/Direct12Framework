#include "stdafx.h"
#include "MainScene.h"
#include"DX12Manager.h"
#include"InputManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include"ResourceManager.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"Transform.h"
#include"Camera.h"

CMainScene::CMainScene()
{
}

void CMainScene::Initialize()
{
}

void CMainScene::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
		::PostQuitMessage(0);
		return;
	}
	if (INPUT.IsKeyDown(KEY_TYPE::E)) {
		INSTANCE(CSceneManager).LoadScene(SCENE_TYPE::MAINSTAGE);
		return;
	}
	if (INPUT.IsKeyDown(KEY_TYPE::Q)) {
		INSTANCE(CSceneManager).ChangeScene(SCENE_TYPE::MENU);
		return;
	}
	CScene::Update();
}