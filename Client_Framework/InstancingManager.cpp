#include "stdafx.h"
#include "InstancingManager.h"
#include"InstancingGroup.h"
#include"InstancingBuffer.h"
#include"ResourceManager.h"
#include"Shader.h"
#include"Camera.h"
#include"Mesh.h"
#include"Material.h"
#include"MeshRenderer.h"

void CInstancingManager::Initialize()
{
	mInstancingBufferOffset = 0;
}

void CInstancingManager::Destroy()
{
	mInstancingGroupMap.clear();
	mInstancingBufferOffset = 0;
}

void CInstancingManager::AddInstancingObject(const InstanceKey& key, std::shared_ptr<class CGameObject> object)
{
	if (!object) return;
	auto mesh = key.mesh;
	auto material = key.material;
	if (!mesh || !material) return;
	auto group = FindInstancingGroup(key);
	if (!group) {
		group = std::make_shared<CInstancingGroup>();
		group->Initialize(INSTANCE_BUFFER_TYPE::OBJECT);
		AddInstancingGroup(key, group);
	}
	group->AddObject(object);
}

void CInstancingManager::AddInstancingObject(std::shared_ptr<class CGameObject> object)
{
	if (!object) return;
	auto meshRenderer = object->GetComponent<CMeshRenderer>();
	if (!meshRenderer) return;
	auto key = meshRenderer->GetInstancingKey();
	auto mesh = key.mesh;
	auto material = key.material;
	if (!mesh || !material) return;
	auto group = FindInstancingGroup(key);
	if (!group) {
		group = std::make_shared<CInstancingGroup>();
		group->Initialize(INSTANCE_BUFFER_TYPE::OBJECT);
		AddInstancingGroup(key, group);
	}
	group->AddObject(object);
}

void CInstancingManager::UpdateInstancingGroup(const std::shared_ptr<class CCamera>& camera)
{
	mInstancingBufferOffset = 0;
	for (auto& [key, group] : mInstancingGroupMap) {
		mInstancingBufferOffset = group->Update(camera, mInstancingBufferOffset);
	}
}

void CInstancingManager::RenderInstancingGroup(int pass)
{
	auto instancingBuffer = INSTANCE(CDX12Manager).GetInstancingBuffer((UINT)INSTANCE_BUFFER_TYPE::OBJECT);
	for (auto& [key, group] : mInstancingGroupMap) {
		int instancingCount = group->mInstancingCnt;
		if (instancingCount == 0) continue;

		auto mesh = key.mesh;
		auto material = key.material;
		if (!mesh || !material) continue;
		if (!material->GetShader((PASS_TYPE)pass)) continue;
		if (mesh->GetSubMeshNum() == 0) continue;
		material->BindShader((PASS_TYPE)pass);
		material->BindDataToShader();

		int startOffset = group->mInstancingBufferOffset;
		auto instancingbufferView = instancingBuffer->GetInstancingBufferView(startOffset, instancingCount);

		mesh->Render(instancingbufferView, instancingCount, 0);

	}
}
