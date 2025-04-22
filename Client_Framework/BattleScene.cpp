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

	INSTANCE(CResourceManager).UpdateMaterials();
	lightMgr->Update();

	auto& mainCam = mCameras["MainCamera"];
	auto& camera = mCameras["DirectinalLight"];

	if(mainCam && camera){
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
	}

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

	
	Vec2 shadowMapSize = { 4096.f * 3, 4096.f * 3 };
	float size = 100.f * 2.f;

	dirLightObj = CGameObject::CreateCameraObject("DirectinalLight", shadowMapSize, 1.f, size, { size, size });
	auto transform = dirLightObj->GetTransform();

	dir.Normalize();
	transform->LookTo(dir);
}
