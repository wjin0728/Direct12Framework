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
#include"InstancingGroup.h"

CBattleScene::CBattleScene()
{
}

void CBattleScene::Initialize()
{
	mRenderLayers[L"Opaque"] = ObjectList{};
	mShaders[L"Opaque"] = RESOURCE.Get<CShader>(L"Forward");
	mRenderLayers[L"Terrain"] = ObjectList{};
	mShaders[L"Terrain"] = RESOURCE.Get<CShader>(L"Terrain");
	mRenderLayers[L"SkyBox"] = ObjectList{};
	mShaders[L"SkyBox"] = RESOURCE.Get<CShader>(L"SkyBox");

//#pragma region Obstacles
//
//	std::random_device rd; 
//	std::mt19937 gen(rd()); 
//	std::uniform_real_distribution<float> disX(-30.f, 30.f); 
//	std::uniform_real_distribution<float> disY(180.f, 200.f);
//	std::uniform_real_distribution<float> disZ(50.f, 1000.f);
//
//	Vec3 size = { 25.f,25.f,25.f };
//
//	auto obstaclePrefab = CGameObject::CreateObjectFromFile(L"Obstacle", L"Resources\\Models\\Stone_1.bin");
//	obstaclePrefab->GetTransform()->SetLocalScale(size);
//	auto obstaclePrefab2 = CGameObject::CreateObjectFromFile(L"Obstacle", L"Resources\\Models\\Stone_2.bin");
//	obstaclePrefab2->GetTransform()->SetLocalScale(size);
//	auto obstaclePrefab3 = CGameObject::CreateObjectFromFile(L"Obstacle", L"Resources\\Models\\Stone_5.bin");
//	obstaclePrefab3->GetTransform()->SetLocalScale(size);
//
//
//	for (int i = 0; i < 100; i++) {
//		auto obstacle = CGameObject::Instantiate(obstaclePrefab);
//		obstacle->SetStatic(true);
//		obstacle->GetTransform()->SetLocalPosition({ disX(gen),disY(gen),disZ(gen) });
//
//		auto obstacle2 = CGameObject::Instantiate(obstaclePrefab2);
//		obstacle2->SetStatic(true);
//		obstacle2->GetTransform()->SetLocalPosition({ disX(gen),disY(gen),disZ(gen) });
//
//		auto obstacle3 = CGameObject::Instantiate(obstaclePrefab3);
//		obstacle3->SetStatic(true);
//		obstacle3->GetTransform()->SetLocalPosition({ disX(gen),disY(gen),disZ(gen) });
//
//		AddObject(L"Opaque", obstacle);
//		AddObject(L"Opaque", obstacle2);
//		AddObject(L"Opaque", obstacle3);
//	}
//
//#pragma endregion

#pragma region Player

	auto Player = CGameObject::CreateObjectFromFile(L"Player", L"Resources\\Models\\HelicopterModel.bin");
	Player->SetStatic(false);
	Player->GetTransform()->SetLocalPosition({ 0.f,200.f,0.f });
	Player->GetTransform()->SetLocalScale({ 1.f, 1.f, 1.f });
	Player->CalculateRootOOBB();

	Player->AddComponent<CRigidBody>();
	Player->AddComponent<CPlayerController>();

	/*auto avoidObstacle = std::make_shared<CAvoidObstacle>();
	Player->AddScript(avoidObstacle);
	avoidObstacle->SetOwner(Player);*/

	AddObject(L"Opaque", Player);

#pragma endregion


#pragma region Main Camera
	{
		auto playerFollower = std::make_shared<CGameObject>();
		playerFollower->SetStatic(false);

		auto followTarget = playerFollower->AddComponent<CFollowTarget>();
		followTarget->SetTarget(Player);
		playerFollower->GetTransform()->SetLocalPosition({ 0.f,200.f,0.f });

		auto cameraObj = CGameObject::CreateCameraObject(L"MainCamera", INSTANCE(CDX12Manager).GetRenderTargetSize(),
			1.f, 2000.f);
		cameraObj->SetStatic(false);
		cameraObj->GetTransform()->SetLocalPosition({ 0.f, 3.f, -6.f });
		cameraObj->GetTransform()->Rotate({ 15.f,0.f,0.f });
		cameraObj->SetParent(playerFollower);

		AddObject(L"Opaque", playerFollower);
	}
#pragma endregion

#pragma region Terrain
	{
		auto terrainObj = CGameObject::CreateTerrainObject(L"Terrain", L"Resources\\Textures\\HeightMap.raw", 
			257, 257, { 8.0f, 1.5f, 8.0f });
		AddObject(L"", terrainObj);

		mTerrain = terrainObj->GetComponent<CTerrain>();

		auto skyBox = CGameObject::CreateRenderObject(L"SkyBox", L"Cube", L"SkyBox");
		skyBox->SetStatic(true);
		skyBox->GetTransform()->SetLocalScale({ 2000.f,2000.f,2000.f });

		AddObject(L"SkyBox", skyBox);
	}
#pragma endregion

#pragma region Billboard
	auto instancingGroup = std::make_shared<CInstancingGroup>();
	instancingGroup->Initialize(INSTANCE_BUFFER_TYPE::BILLBOARD);
	instancingGroups.push_back(instancingGroup);



	for (float i = -2000.f; i < 2000.f; i+=50.f) {
		for (float j = -2000.f; j < 2000.f; j+=50.f) {
			float width = 50.f;
			float height = 50.f;

			float halfWidth = 0.5f * width;
			float halfHeight = 0.5f * height;

			auto billboard = CGameObject::CreateRenderObject(L"Billboard", L"", L"Tree2");
			billboard->SetStatic(true);
			billboard->SetInstancing(true);
			billboard->GetTransform()->SetLocalPosition({ i,mTerrain->GetHeight(i, j) + halfHeight,j});
			billboard->GetTransform()->SetLocalScale({ halfWidth, halfHeight, 1.f });

			AddObject(L"", billboard);
			instancingGroup->AddObject(billboard);
		}
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

	for (const auto& instancingGroup : instancingGroups) {
		instancingGroup->Render(CCamera::GetMainCamera());
	}
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
