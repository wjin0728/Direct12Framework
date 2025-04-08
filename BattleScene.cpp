#include "stdafx.h"
#include "BattleScene.h"
#include"DX12Manager.h"
#include "Scene.h"
#include "SceneManager.h"
#include"ResourceManager.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"FollowTarget.h"
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

	auto Player = std::make_shared<CGameObject>();
	Player->SetStatic(false);
	Player->GetTransform()->SetLocalPosition({ 0.f,10.f,0.f });
	Player->GetTransform()->SetLocalScale({ 1.f, 1.f, 1.f });
	//Player->GetTransform()->SetLocalRotationY(90.f);

	Player->AddComponent<CRigidBody>();
	Player->AddComponent<CPlayerController>();

	AddObject("", Player);

#pragma endregion

#pragma region Main Camera
	{
		auto playerFollower = std::make_shared<CGameObject>();
		playerFollower->SetStatic(false);

		auto followTarget = playerFollower->AddComponent<CFollowTarget>();
		followTarget->SetTarget(Player);
		playerFollower->GetTransform()->SetLocalPosition({ 0.f,10.f,0.f });
		//playerFollower->GetTransform()->SetLocalRotationY(90.f);

		auto cameraObj = CGameObject::CreateCameraObject("MainCamera", INSTANCE(CDX12Manager).GetRenderTargetSize(),
			1.f, 100.f);
		cameraObj->SetStatic(false);
		cameraObj->GetTransform()->SetLocalPosition({ 0.f, 0.f, 0.f });
		cameraObj->GetTransform()->Rotate({ 15.f,0.f,0.f });
		cameraObj->SetParent(playerFollower);

		AddObject("", playerFollower);

		auto uiCamera = CGameObject::CreateCameraObject("UICamera", INSTANCE(CDX12Manager).GetRenderTargetSize(), 0.f, 100.f, INSTANCE(CDX12Manager).GetRenderTargetSize());
	}
#pragma endregion

	SetLights();

	auto shadowMap = RESOURCE.Get<CTexture>("ShadowMap");
	shadowMap->CreateSRV();
	shadowMap->ChangeResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void CBattleScene::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
		::PostQuitMessage(0);
		return;
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
	transform->LookTo(dir);
	camera->GenerateViewMatrix();

	UpdatePassData();
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

	dirLightObj = std::make_shared<CGameObject>();
	dirLightObj->SetTag("DirectinalLight");
	auto camera = std::make_shared<CCamera>();
	dirLightObj->AddComponent(camera);

	Vec2 shadowMapSize = { 4096.f * 3, 4096.f * 3 };
	camera->SetViewport(0, 0, shadowMapSize.x, shadowMapSize.y);
	camera->SetScissorRect(0, 0, shadowMapSize.x, shadowMapSize.y);

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
	
	camera->GenerateOrthographicProjectionMatrix(1.f, r*2.f, r * 2.f, r * 2.f);

	AddCamera(camera);

	auto transform = dirLightObj->GetTransform();

	dir.Normalize();
	transform->SetLocalPosition(center - (r*dir));
	transform->LookTo(dir);

	mObjects.push_back(dirLightObj);
}
