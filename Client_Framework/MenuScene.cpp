#include "stdafx.h"
#include "MenuScene.h"
#include"DX12Manager.h"
#include"InputManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include"ResourceManager.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"Transform.h"
#include"Camera.h"

CMenuScene::CMenuScene()
{
}

void CMenuScene::Initialize()
{
}

void CMenuScene::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
		INSTANCE(CSceneManager).ReturnPrevScene();
		return;
	}
	CScene::Update();
}