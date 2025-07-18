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

CBattleScene::CBattleScene() : CScene()
{
}

void CBattleScene::Initialize()
{
	// Load default resources
	INSTANCE(ServerManager).Client_Login();
	INSTANCE(ServerManager).RegisterPlayerInScene(this);
	LoadSceneFromFile(SCENE_PATH("Scene"));
	CLight::SetVolumes();
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

	}
	if (INPUT.IsKeyDown(KEY_TYPE::F3)) {

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
}

