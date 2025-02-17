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
	mShaders[L"Shadow"] = RESOURCE.Get<CShader>(L"Shadow");
	mRenderLayers[L"UI"] = ObjectList{};
	mShaders[L"UI"] = RESOURCE.Get<CShader>(L"Sprite");

#pragma region Obstacles

	std::random_device rd; 
	std::mt19937 gen(rd()); 
	std::uniform_real_distribution<float> disX(-30.f, 30.f); 
	std::uniform_real_distribution<float> disY(180.f, 200.f);
	std::uniform_real_distribution<float> disZ(50.f, 1000.f);

	Vec3 size = { 25.f,25.f,25.f };

	auto obstaclePrefab = CGameObject::CreateObjectFromFile(L"Obstacle", L"Resources\\Models\\Stone_1.bin");
	obstaclePrefab->GetTransform()->SetLocalScale(size);
	auto obstaclePrefab2 = CGameObject::CreateObjectFromFile(L"Obstacle", L"Resources\\Models\\Stone_2.bin");
	obstaclePrefab2->GetTransform()->SetLocalScale(size);
	auto obstaclePrefab3 = CGameObject::CreateObjectFromFile(L"Obstacle", L"Resources\\Models\\Stone_5.bin");
	obstaclePrefab3->GetTransform()->SetLocalScale(size);


	for (int i = 0; i < 100; i++) {
		auto obstacle = CGameObject::Instantiate(obstaclePrefab);
		obstacle->SetStatic(true);
		obstacle->GetTransform()->SetLocalPosition({ disX(gen),disY(gen),disZ(gen) });

		auto obstacle2 = CGameObject::Instantiate(obstaclePrefab2);
		obstacle2->SetStatic(true);
		obstacle2->GetTransform()->SetLocalPosition({ disX(gen),disY(gen),disZ(gen) });

		auto obstacle3 = CGameObject::Instantiate(obstaclePrefab3);
		obstacle3->SetStatic(true);
		obstacle3->GetTransform()->SetLocalPosition({ disX(gen),disY(gen),disZ(gen) });

		AddObject(L"Opaque", obstacle);
		AddObject(L"Opaque", obstacle2);
		AddObject(L"Opaque", obstacle3);
	}

#pragma endregion

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
			1.f, 300.f);
		cameraObj->SetStatic(false);
		cameraObj->GetTransform()->SetLocalPosition({ 0.f, 4.f, -8.f });
		cameraObj->GetTransform()->Rotate({ 15.f,0.f,0.f });
		cameraObj->SetParent(playerFollower);

		AddObject(L"", playerFollower);
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
	
	
	
	for (float i = -10.f; i < 10.f; i+=250.f) {
		for (float j = -10.f; j < 10.f; j+=250.f) {
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

	std::shared_ptr<CMaterial> mat = std::make_shared<CMaterial>();
	mat->SetName(L"ShadowMap");
	mat->mNormalMapIdx = 0;
	mat->mDiffuseMapIdx = RESOURCE.Get<CTexture>(L"ShadowMap")->CreateSRV();
	RESOURCE.Add(mat);

#pragma region UI

#pragma endregion


	SetLights();

	auto shadowMap = RESOURCE.Get<CTexture>(L"ShadowMap");
	shadowMap->ChangeResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void CBattleScene::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
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
	for (const auto& object : mObjects) {
		object->LateUpdate();
	}

	INSTANCE(CResourceManager).UpdateMaterials();
	lightMgr->Update();

	auto& mainCam = mCameras[L"MainCamera"];
	auto& camera = mCameras[L"DirectinalLight"];

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

void CBattleScene::SetLights()
{
	lightMgr = std::make_unique<CLightManager>();
	lightMgr->Initialize();

	Vec4 lightColor = { 1.f,1.f,1.f,1.f };
	Vec3 strength = { 1.f,1.f,1.f };
	Vec3 dir = { 1.f,-1.5f,1.f };

	dirLight = std::make_shared<CDirectionalLight>(lightColor, strength, dir);
	lightMgr->AddDirectionalLight(dirLight);

	dirLightObj = std::make_shared<CGameObject>();
	dirLightObj->SetTag(L"DirectinalLight");
	auto camera = std::make_shared<CCamera>();
	dirLightObj->AddComponent(camera);
	camera->SetViewport(0, 0, 4048, 4048);
	camera->SetScissorRect(0, 0, 4048, 4048);

	auto& mainCam = mCameras[L"MainCamera"];
	Vec3 corners[8]{};
	mainCam->mFrustumView.GetCorners(corners);

	float fov = mainCam->GetFov();
	float tanHalfVFov = tanf(XMConvertToRadians(fov / 2.0f));
	float tanHalfHFov = tanHalfVFov * mainCam->GetAspect();

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

	AddObject(dirLightObj);
}
