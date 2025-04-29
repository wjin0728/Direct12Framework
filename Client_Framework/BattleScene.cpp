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

CBattleScene::CBattleScene()
{
}

void CBattleScene::Initialize()
{
	LoadSceneFromFile(SCENE_PATH(std::string("Lobby1")));
#pragma region Player

	auto Player = FindObjectWithTag("Player");
	Player->SetStatic(false);

	Player->AddComponent<CRigidBody>();
	auto playerController = Player->AddComponent<CPlayerController>();

#pragma endregion

#pragma region Main Camera
	{

		auto cameraObj = CGameObject::CreateCameraObject("MainCamera", INSTANCE(CDX12Manager).GetRenderTargetSize(),
			1.f, 50.f);
		cameraObj->SetStatic(false);

		auto playerFollower = cameraObj->AddComponent<CThirdPersonCamera>();
		playerFollower->SetTarget(Player);

		AddObject(cameraObj);
		playerController->SetCamera(cameraObj->GetComponent<CCamera>());

		auto uiCamera = CGameObject::CreateCameraObject("UICamera", INSTANCE(CDX12Manager).GetRenderTargetSize(), 0.f, 100.f, INSTANCE(CDX12Manager).GetRenderTargetSize());
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

