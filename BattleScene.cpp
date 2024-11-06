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
#include"AvoidObstacle.h"
#include"InputManager.h"
#include"Terrain.h"
#include"Camera.h"

CBattleScene::CBattleScene()
{
}

void CBattleScene::Initialize()
{
	mObjects[L"Opaque"] = ObjectList{};
	mShaders[L"Opaque"] = RESOURCE.Get<CShader>(L"Forward");
	mObjects[L"Terrain"] = ObjectList{};
	mShaders[L"Terrain"] = RESOURCE.Get<CShader>(L"Terrain");
	mObjects[L"SkyBox"] = ObjectList{};
	mShaders[L"SkyBox"] = RESOURCE.Get<CShader>(L"SkyBox");

#pragma region Obstacles

	std::random_device rd; 
	std::mt19937 gen(rd()); 
	std::uniform_real_distribution<float> disX(-30.f, 30.f); 
	std::uniform_real_distribution<float> disY(180.f, 200.f);
	std::uniform_real_distribution<float> disZ(50.f, 1000.f);

	Vec3 size = { 25.f,25.f,25.f };
	Vec3 colliderSize = { 1.4f, 1.4f, 1.4f };

	auto obstaclePrefab = CGameObject::CreateObjectFromFile(L"Obstacle", L"Resources\\Models\\Stone_1.bin");
	obstaclePrefab->GetTransform()->SetLocalScale(size);
	obstaclePrefab->GetCollider()->SetScale(colliderSize);
	auto obstaclePrefab2 = CGameObject::CreateObjectFromFile(L"Obstacle", L"Resources\\Models\\Stone_2.bin");
	obstaclePrefab2->GetTransform()->SetLocalScale(size);
	obstaclePrefab2->GetCollider()->SetScale(colliderSize);
	auto obstaclePrefab3 = CGameObject::CreateObjectFromFile(L"Obstacle", L"Resources\\Models\\Stone_5.bin");
	obstaclePrefab3->GetTransform()->SetLocalScale(size);
	obstaclePrefab3->GetCollider()->SetScale(colliderSize);


	for (int i = 0; i < 100; i++) {
		auto obstacle = CGameObject::Instantiate(obstaclePrefab);
		obstacle->GetTransform()->SetLocalPosition({ disX(gen),disY(gen),disZ(gen) });
		mObjects[L"Opaque"].push_back(obstacle);

		auto obstacle2 = CGameObject::Instantiate(obstaclePrefab2);
		obstacle2->GetTransform()->SetLocalPosition({ disX(gen),disY(gen),disZ(gen) });
		mObjects[L"Opaque"].push_back(obstacle2);

		auto obstacle3 = CGameObject::Instantiate(obstaclePrefab3);
		obstacle3->GetTransform()->SetLocalPosition({ disX(gen),disY(gen),disZ(gen) });
		mObjects[L"Opaque"].push_back(obstacle3);
	}

#pragma endregion

#pragma region Player

	auto Player = CGameObject::CreateObjectFromFile(L"Player", L"Resources\\Models\\SuperCobra.bin");
	Player->GetTransform()->SetLocalPosition({ 0.f,200.f,0.f });
	Player->GetTransform()->SetLocalScale({ 0.4f, 0.4f, 0.4f });
	Player->CalculateRootOOBB();

	auto rigidBody = std::make_shared<CRigidBody>();
	Player->AddComponent(rigidBody);
	rigidBody->SetOwner(Player);

	auto playerController = std::make_shared<CPlayerController>();
	Player->AddScript(playerController);
	playerController->SetOwner(Player);

	auto avoidObstacle = std::make_shared<CAvoidObstacle>();
	Player->AddScript(avoidObstacle);
	avoidObstacle->SetOwner(Player);

	mObjects[L"Opaque"].push_back(Player);

#pragma endregion


#pragma region Main Camera
	{
		auto playerFollower = std::make_shared<CGameObject>();
		auto followTarget = std::make_shared<CFollowTarget>();
		playerFollower->AddScript(followTarget);
		followTarget->SetTarget(Player);
		playerFollower->SetComponentOwner(playerFollower);
		playerFollower->GetTransform()->SetLocalPosition({ 0.f,200.f,0.f });

		auto cameraObj = CGameObject::CreateCameraObject(L"MainCamera", INSTANCE(CDX12Manager).GetRenderTargetSize(),
			1.f, 1000.f);
		cameraObj->GetTransform()->SetLocalPosition({ 0.f, 3.f, -6.f });
		cameraObj->GetTransform()->Rotate({ 15.f,0.f,0.f });
		cameraObj->SetParent(playerFollower);

		mObjects[L"Opaque"].push_back(playerFollower);
	}
#pragma endregion

#pragma region Terrain
	{
		auto terrainObj = CGameObject::CreateTerrainObject(L"Terrain", L"Resources\\Textures\\HeightMap.raw", 
			257, 257, { 8.0f, 1.5f, 8.0f });
		mObjects[L"Terrain"].push_back(terrainObj);

		mTerrain = std::dynamic_pointer_cast<CTerrain>(terrainObj->GetComponent(COMPONENT_TYPE::TERRAIN));

		auto skyBox = CGameObject::CreateRenderObject(L"SkyBox", L"Cube", L"SkyBox");
		skyBox->GetTransform()->SetLocalScale({ 2000.f,2000.f,2000.f });
		mObjects[L"SkyBox"].push_back(skyBox);
	}
#pragma endregion

	SetLights();
}

void CBattleScene::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
		CCamera::DeleteMainCamera();
		INSTANCE(CSceneManager).LoadScene(SCENE_TYPE::MAIN);
		return;
	}
	if (INPUT.IsKeyDown(KEY_TYPE::Q)) {
		INSTANCE(CSceneManager).ChangeScene(SCENE_TYPE::MENU);
		return;
	}
	CScene::Update();
}

void CBattleScene::LateUpdate()
{
	CScene::LateUpdate();
	lightMgr->Update();
}

void CBattleScene::Render()
{
	PrepareRender();
	RenderForLayer(L"SkyBox", false);
	RenderForLayer(L"Opaque");
	RenderTerrain(L"Terrain");
}

void CBattleScene::SetLights()
{
	lightMgr = std::make_unique<CLightManager>();
	lightMgr->Initialize();

	Vec4 lightColor = { 1.f,1.f,1.f,1.f };
	Vec3 strength = { 1.f,1.f,1.f };
	Vec3 dir = { 1.f,-1.f,1.f };

	std::shared_ptr<CDirectionalLight> dirLight = std::make_shared<CDirectionalLight>(lightColor, strength, dir);

	lightMgr->AddDirectionalLight(dirLight);
}
