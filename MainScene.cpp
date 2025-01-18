#include "stdafx.h"
#include "MainScene.h"
#include"DX12Manager.h"
#include"InputManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include"ResourceManager.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"Transform.h"
#include"Camera.h"

CMainScene::CMainScene()
{
}

void CMainScene::Initialize()
{
	mRenderLayers[L"UI"] = ObjectList{};
	mShaders[L"UI"] = RESOURCE.Get<CShader>(L"Sprite");

	auto cameraObj = CGameObject::CreateCameraObject(L"MainCamera", INSTANCE(CDX12Manager).GetRenderTargetSize());
	AddObject(L"UI", cameraObj);

	const auto& rectMesh = RESOURCE.Get<CMesh>(L"Rectangle");
	{
		auto object = std::make_shared<CGameObject>();

		auto renderer = object->AddComponent<CMeshRenderer>();
		renderer->SetMesh(rectMesh);
		renderer->AddMaterial(RESOURCE.Get<CMaterial>(L"MainMenu"));

		object->SetActive(true);
		object->SetRenderLayer(L"UI");

		AddObject(L"UI", object);
	}
}

void CMainScene::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
		::PostQuitMessage(0);
		return;
	}
	if (INPUT.IsKeyDown(KEY_TYPE::E)) {
		INSTANCE(CSceneManager).LoadScene(SCENE_TYPE::MAINSTAGE);
		return;
	}
	if (INPUT.IsKeyDown(KEY_TYPE::Q)) {
		INSTANCE(CSceneManager).ChangeScene(SCENE_TYPE::MENU);
		return;
	}
	CScene::Update();
}