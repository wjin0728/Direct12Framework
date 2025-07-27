#include "stdafx.h"
#include "LobbyScene.h"
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
#include"Light.h"
#include"ObjectState.h"
#include"ClassSelect.h"

CLobbyScene::CLobbyScene()
{
}

void CLobbyScene::Initialize()
{// Load default resources
	INSTANCE(ServerManager).Client_Login();
	INSTANCE(ServerManager).RegisterPlayerInScene(this);
	LoadSceneFromFile(SCENE_PATH("Lobby"));
	CLight::SetVolumes();

	auto lobbyUI = RESOURCE.GetPrefab("LobbyUI");
	if (lobbyUI) {
		auto uiObject = CGameObject::Instantiate(lobbyUI);
		uiObject->SetName("LobbyUI");
		uiObject->AddComponent<CClassSelectUI>();
		AddObjectImmediately(uiObject);
	}

	auto mainPlayer = FindObjectWithName("MainPlayer");
	if (mainPlayer) {
		auto playerController = mainPlayer->GetComponent<CPlayerController>();
		if (playerController) {
			playerController->ChangeControllMode(CPlayerController::ControllMode::ClassSelection);
		}
	}
	if(auto camera = FindObjectWithName("MainCamera")) {
		if(auto cameraCmp = camera->GetComponent<CCamera>()) {
			float size = 2.f * 2.f;
			Vec2 rtSize = INSTANCE(CDX12Manager).GetRenderTargetSize();
			float aspect = rtSize.x / rtSize.y;
			//cameraCmp->GenerateReverseZPerspectiveProjectionMatrix(0.1f, 130.f, 60.f);
			cameraCmp->GenerateReverseZOrthographicProjectionMatrix(0.5f, 100.f, size * aspect, size);
			cameraCmp->SetCameraType(CCamera::CameraType::Orthographic);
		}
		auto transform = camera->GetTransform();
		if (transform) {
			transform->SetLocalPosition	({ 5.182, 2.994, 7.055 });
			transform->SetLocalRotation({ 20.578, 49.1, 0.f });
		}
		auto thirdPersonCamera = camera->GetComponent<CThirdPersonCamera>();
		if (thirdPersonCamera) {
			//thirdPersonCamera->SetDefaultCameraParams();
			thirdPersonCamera->ChangeCameraMode(CThirdPersonCamera::CameraMode::FixedPosition);
		}
	}


	

	INPUT.FixMousePosition(false);
}

void CLobbyScene::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
		::PostQuitMessage(0);
		return;
	}
	else if (INPUT.IsKeyDown(KEY_TYPE::ALT)) {
		INPUT.ChangeMouseState();
	}
	/*for (int i = 0; i < 7; i++) {
		int key = (int)KEY_TYPE::ONE + i;
		if (INPUT.IsKeyDown((KEY_TYPE)key))
		{
			if (renderTargetIndices.size() > i)
				renderPasstype = i;
		}
	}*/
	/*else if (INPUT.IsKeyDown(KEY_TYPE::L)) {
		INPUT.ChangeMouseState();
	}*/
	CScene::Update();
}

void CLobbyScene::LateUpdate()
{
	CScene::LateUpdate();
}

void CLobbyScene::RenderScene()
{
	mRenderMgr->RenderShadowPass();
	mRenderMgr->RenderGBufferPass();
	mRenderMgr->RenderLightingPass();
	mRenderMgr->RenderForwardPass();
	mRenderMgr->RenderFinalPass();
	mRenderMgr->RenderUIPass();
}
