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
	LoadSceneFromFile(SCENE_PATH(std::string("Lobby1")));
#pragma region Player

	auto Player = FindObjectWithTag("Player");
	Player->SetStatic(false);

	Player->AddComponent<CRigidBody>();
	auto playerController = Player->AddComponent<CPlayerController>();

#pragma endregion

	auto shadowMap = RESOURCE.Get<CTexture>("ShadowMap");
	shadowMap->CreateSRV();
	shadowMap->ChangeResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
	}
#pragma endregion
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
	for (const auto& object : mObjects) {
		object->LateUpdate();
	}
	for (const auto& player : mPlayers) {
		if (player) player->LateUpdate();
	}
	UpdatePassData();
}

void CBattleScene::RenderScene()
{
	CScene::RenderShadowPass();
	CScene::RenderGBufferPass();
	CScene::RenderLightingPass();
	//CScene::RenderForwardPass();
	CScene::RenderFinalPass();
}

