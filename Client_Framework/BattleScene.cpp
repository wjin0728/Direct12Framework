#include "stdafx.h"
#include "BattleScene.h"
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

CBattleScene::CBattleScene()
{
}

void CBattleScene::Initialize()
{
	// Load default resources
	INSTANCE(ServerManager).RegisterPlayerInScene(this);
	LoadSceneFromFile(SCENE_PATH("Lobby1"));
}

void CBattleScene::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
		::PostQuitMessage(0);
		return;
	}
	else if (INPUT.IsKeyDown(KEY_TYPE::ALT)) {
		INPUT.ChangeMouseState();
	}
	/*else if (INPUT.IsKeyDown(KEY_TYPE::L)) {
		INPUT.ChangeMouseState();
	}*/
	CScene::Update();

}

void CBattleScene::LateUpdate()
{
	CScene::LateUpdate();
}

void CBattleScene::RenderScene()
{
	CScene::RenderShadowPass();
	CScene::RenderGBufferPass();
	CScene::RenderLightingPass();
	//CScene::RenderForwardPass();
	CScene::RenderFinalPass();
}

