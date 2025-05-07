#include "stdafx.h"
#include "MiniGame.h"
#include"DX12Manager.h"
#include "Scene.h"
#include "SceneManager.h"
#include"ResourceManager.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"ThirdPersonCamera.h"
#include"Transform.h"
#include"RigidBody.h"
#include"Collider.h"
#include"PlayerController.h"
#include"InputManager.h"
#include"Terrain.h"
#include"Camera.h"
#include"Material.h"
#include"InstancingGroup.h"
#include"ServerManager.h"

CMiniGame::CMiniGame()
{
}

void CMiniGame::Initialize()
{
	INSTANCE(ServerManager).RegisterPlayerInScene(this);
	LoadSceneFromFile(SCENE_PATH("MiniGame1"));
}

void CMiniGame::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
		::PostQuitMessage(0);
		return;
	}
	else if (INPUT.IsKeyDown(KEY_TYPE::ALT)) {
		INPUT.ChangeMouseState();
	}
	CScene::Update();
}

void CMiniGame::LateUpdate()
{
	CScene::LateUpdate();
}

void CMiniGame::RenderScene()
{
	CScene::RenderShadowPass();
	CScene::RenderGBufferPass();
	CScene::RenderLightingPass();
	//CScene::RenderForwardPass();
	CScene::RenderFinalPass();
}
