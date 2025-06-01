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

CLobbyScene::CLobbyScene()
{
}

void CLobbyScene::Initialize()
{// Load default resources
	INSTANCE(ServerManager).Client_Login();
	INSTANCE(ServerManager).RegisterPlayerInScene(this);
	LoadSceneFromFile(SCENE_PATH("Lobby"));
	CLight::SetVolumes();
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
	for (int i = 0; i < 7; i++) {
		int key = (int)KEY_TYPE::ONE + i;
		if (INPUT.IsKeyDown((KEY_TYPE)key))
		{
			if (renderTargetIndices.size() > i)
				renderPasstype = i;
		}
	}
	if (INPUT.IsKeyDown(KEY_TYPE::F2)) {

	}
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
	CScene::RenderShadowPass();
	CScene::RenderGBufferPass();
	CScene::RenderLightingPass();
	CScene::RenderForwardPass();
	CScene::RenderFinalPass();
}
