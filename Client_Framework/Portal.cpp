#include "stdafx.h"
#include "Portal.h"
#include"DX12Manager.h"
#include "MeshRenderer.h"
#include"ResourceManager.h"
#include"Mesh.h"
#include"Material.h"
#include"Transform.h"
#include"Camera.h"
#include"Shader.h"
#include"InstancingBuffer.h"
#include"ObjectPoolManager.h"
#include"RenderManager.h"
#include"GameObject.h"


CPortal::CPortal()
{
}

CPortal::~CPortal()
{
}

void CPortal::Awake()
{
}

void CPortal::Start()
{
	if (auto renderer = owner->GetComponent<CMeshRenderer>()) {
		if (auto mat = RESOURCE.Get<CMaterial>("Portal")->Instantiate()) {
			mat->EnrollToPool();
			mat->SetProperty("mainColor", Color(0.f, 0.164f, 1.f, 0.5f));
			UINT texIdx = RESOURCE.GetTextureIndex("Portal_Tex_01");
			mat->SetProperty("mainTexIdx", texIdx);
			renderer->SetMaterial(mat);
		}
	}
}

void CPortal::Update()
{
}
