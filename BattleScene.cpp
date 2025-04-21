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

	SetLights();
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
	for (const auto& object : mObjects) {
		object->LateUpdate();
	}

	INSTANCE(CResourceManager).UpdateMaterials();
	lightMgr->Update();

	auto& mainCam = mCameras["MainCamera"];
	auto& camera = mCameras["DirectinalLight"];

	Vec3 corners[8]{};
	mainCam->mFrustumWorld.GetCorners(corners);

	Vec3 center{};
	for (int i = 0; i < 8; ++i) {
		center += corners[i];
	}
	center /= 8.f;

	float max = -FLT_MAX;
	for (int i = 0; i < 8; ++i) {
		if (max < Vec3::Distance(center, corners[i])) max = Vec3::Distance(center, corners[i]);
	}
	float r = max;

	auto transform = dirLightObj->GetTransform();

	Vec3 dir = dirLight->direction;
	dir.Normalize();
	transform->SetLocalPosition(center - (r * dir));
	transform->LookTo(dir);
	camera->GenerateViewMatrix();

	UpdatePassData();
}

void CBattleScene::RenderScene()
{
	CScene::RenderShadowPass();
	CScene::RenderForwardPass();
}

void CBattleScene::SetLights()
{
	lightMgr = std::make_unique<CLightManager>();
	lightMgr->Initialize();

	Vec3 lightColor = { 1.f,1.f,1.f};
	Vec3 strength = { 1.f,1.f,1.f };
	Vec3 dir = { 0.5f,-1.f,0.5f };

	dirLight = std::make_shared<CDirectionalLight>(lightColor, strength, dir);
	lightMgr->AddDirectionalLight(dirLight);

	auto& mainCam = mCameras["MainCamera"];
	Vec3 corners[8]{};
	mainCam->mFrustumView.GetCorners(corners);


	Vec3 center{};
	for (int i = 0; i < 8; ++i) {
		center += corners[i];
	}
	center /= 8.f;
	float max = -FLT_MAX;
	for (int i = 0; i < 8; ++i) {
		if (max < Vec3::Distance(center, corners[i])) max = Vec3::Distance(center, corners[i]);
	}
	float r = max;
	
	Vec2 shadowMapSize = { 4096.f * 3, 4096.f * 3 };
	float size = r * 2.f;

	dirLightObj = CGameObject::CreateCameraObject("DirectinalLight", shadowMapSize, 1.f, size, { size, size });
	auto transform = dirLightObj->GetTransform();

	dir.Normalize();
	transform->SetLocalPosition(center - (r*dir));
	transform->LookTo(dir);
}
