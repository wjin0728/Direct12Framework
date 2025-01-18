#include "stdafx.h"
#include "MenuScene.h"
#include"DX12Manager.h"
#include"InputManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include"ResourceManager.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"Transform.h"
#include"ScrollTexture.h"
#include"Camera.h"

CMenuScene::CMenuScene()
{
}

void CMenuScene::Initialize()
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
		renderer->AddMaterial(RESOURCE.Get<CMaterial>(L"Scrolling"));

		object->GetTransform()->SetLocalScale({ 0.5f, 10.f,1.f });
		object->AddComponent<CScrollTexture>();
		object->SetActive(true);
		object->SetRenderLayer(L"UI");
		AddObject(L"UI", object);
	}
}

void CMenuScene::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
		INSTANCE(CSceneManager).ReturnPrevScene();
		return;
	}
	CScene::Update();
}