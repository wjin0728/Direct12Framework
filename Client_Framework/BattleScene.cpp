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
#include"ParticleManager.h"
#include"Light.h"
#include"RenderManager.h"
#include"CEntityState.h"
#include"UIController.h"
#include "ParticleAttach.h"

CBattleScene::CBattleScene() : CScene()
{
}

void CBattleScene::Initialize()
{
	// Load default resources
	INSTANCE(ServerManager).Client_Login();
	INSTANCE(ServerManager).RegisterPlayerInScene(this);
	LoadSceneFromFile(SCENE_PATH("Battle1"));

	auto mainUI = RESOURCE.GetPrefab("MainUI");
	if (mainUI) {
		auto uiObject = CGameObject::Instantiate(mainUI);
		uiObject->SetName("MainUI");
		uiObject->AddComponent<CPlayerHUD>(1);
		AddObjectImmediately(uiObject);
	}
	/*auto portal = RESOURCE.GetPrefab("Portal");
	if (portal) {
		auto portalObject = CGameObject::Instantiate(portal);
		portalObject->GetTransform()->SetLocalPosition(XMFLOAT3(47.92172f, 6.699f, 36.38293f));
		auto particle = portalObject->GetComponent<CParticleAttach>();
		particle->SetLoop(true);

		AddObjectImmediately(portalObject);
	}*/
	CLight::SetVolumes();


	INPUT.FixMousePosition(true);
	FadeOut(0.5f, { 0.0,0.0,0.0,1.f });
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
	/*for (int i = 0; i < 7; i++) {
		int key = (int)KEY_TYPE::ONE + i;
		if (INPUT.IsKeyDown((KEY_TYPE)key))
		{
			if (renderTargetIndices.size() > i)
				renderPasstype = i;
		}
	}*/
	if (INPUT.IsKeyDown(KEY_TYPE::F2)) {
		auto& monsters = GetObjectsWithType(OBJECT_TYPE::ENEMY);
		for (auto& monster : monsters) {
			monster->GetStateMachine()->GetHit(10.f);
			INSTANCE(ServerManager).send_hp_packet(monster->mID, 10);
			std::cout << "Monster ID: " << monster->mID << " took 10 damage." << std::endl;
		}
	}
	if (INPUT.IsKeyDown(KEY_TYPE::F3)) {
		auto& monsters = GetObjectsWithType(OBJECT_TYPE::ENEMY);
		for (auto& monster : monsters) {
			monster->GetStateMachine()->Heal(10.f);
		}
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
	mRenderMgr->RenderShadowPass();
	mRenderMgr->RenderGBufferPass();
	mRenderMgr->RenderLightingPass();
	mRenderMgr->RenderForwardPass();
	mRenderMgr->RenderFinalPass();
	mRenderMgr->RenderUIPass();
}


CBattle2Scene::CBattle2Scene() : CScene()
{
}

void CBattle2Scene::Initialize()
{
	// Load default resources
	INSTANCE(ServerManager).Client_Login();
	INSTANCE(ServerManager).RegisterPlayerInScene(this);
	LoadSceneFromFile(SCENE_PATH("Battle2"));
	CLight::SetVolumes();

	auto mainUI = RESOURCE.GetPrefab("MainUI");
	if (mainUI) {
		auto uiObject = CGameObject::Instantiate(mainUI);
		uiObject->SetName("MainUI");
		uiObject->AddComponent<CPlayerHUD>(2);
		AddObjectImmediately(uiObject);
	}

	INPUT.FixMousePosition(true);
	FadeOut(0.5f, { 0.0,0.0,0.0,1.f });
}

void CBattle2Scene::Update()
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
	if (INPUT.IsKeyDown(KEY_TYPE::F2)) {
		auto& monsters = GetObjectsWithType(OBJECT_TYPE::ENEMY);
		for (auto& monster : monsters) {
			monster->GetStateMachine()->GetHit(10.f);
			INSTANCE(ServerManager).send_hp_packet(monster->mID, 10);
			std::cout << "Monster ID: " << monster->mID << " took 10 damage." << std::endl;
		}
	}
	if (INPUT.IsKeyDown(KEY_TYPE::F3)) {
		auto& monsters = GetObjectsWithType(OBJECT_TYPE::ENEMY);
		for (auto& monster : monsters) {
			monster->GetStateMachine()->Heal(10.f);
		}
	}
	/*else if (INPUT.IsKeyDown(KEY_TYPE::L)) {
		INPUT.ChangeMouseState();
	}*/
	CScene::Update();

}

void CBattle2Scene::LateUpdate()
{
	CScene::LateUpdate();
}

void CBattle2Scene::RenderScene()
{
	mRenderMgr->RenderShadowPass();
	mRenderMgr->RenderGBufferPass();
	mRenderMgr->RenderLightingPass();
	mRenderMgr->RenderForwardPass();
	mRenderMgr->RenderFinalPass();
	mRenderMgr->RenderUIPass();
}


CBattle3Scene::CBattle3Scene() : CScene()
{
}

void CBattle3Scene::Initialize()
{
	// Load default resources
	INSTANCE(ServerManager).Client_Login();
	INSTANCE(ServerManager).RegisterPlayerInScene(this);
	LoadSceneFromFile(SCENE_PATH("Battle3"));
	CLight::SetVolumes();

	auto mainUI = RESOURCE.GetPrefab("MainUI");
	if (mainUI) {
		auto uiObject = CGameObject::Instantiate(mainUI);
		uiObject->SetName("MainUI");
		uiObject->AddComponent<CPlayerHUD>(3);
		AddObjectImmediately(uiObject);
	}

	INPUT.FixMousePosition(true);
	FadeOut(0.5f, { 0.0,0.0,0.0,1.f });
}

void CBattle3Scene::Update()
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
	if (INPUT.IsKeyDown(KEY_TYPE::F2)) {
		auto& monsters = GetObjectsWithType(OBJECT_TYPE::ENEMY);
		for (auto& monster : monsters) {
			monster->GetStateMachine()->GetHit(10.f);
			INSTANCE(ServerManager).send_hp_packet(monster->mID, 10);
			std::cout << "Monster ID: " << monster->mID << " took 10 damage." << std::endl;
		}
	}
	if (INPUT.IsKeyDown(KEY_TYPE::F3)) {
		auto& monsters = GetObjectsWithType(OBJECT_TYPE::ENEMY);
		for (auto& monster : monsters) {
			monster->GetStateMachine()->Heal(10.f);
		}
	}
	/*else if (INPUT.IsKeyDown(KEY_TYPE::L)) {
		INPUT.ChangeMouseState();
	}*/
	CScene::Update();

}

void CBattle3Scene::LateUpdate()
{
	CScene::LateUpdate();
}

void CBattle3Scene::RenderScene()
{
	mRenderMgr->RenderShadowPass();
	mRenderMgr->RenderGBufferPass();
	mRenderMgr->RenderLightingPass();
	mRenderMgr->RenderForwardPass();
	mRenderMgr->RenderFinalPass();
	mRenderMgr->RenderUIPass();
}

