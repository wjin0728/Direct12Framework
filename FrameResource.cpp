#include "stdafx.h"
#include"DX12Manager.h"
#include"FrameResource.h"
#include"ResourceManager.h"
#include"InstancingBuffer.h"
#include"Material.h"
#include"SkinnedMesh.h"

CFrameResource::CFrameResource()
{
	ThrowIfFailed(DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)));

	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::PASS)] = std::make_unique<CConstantBuffer>();
	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::PASS)]->Initialize(0, ALIGNED_SIZE(sizeof(CBPassData)) * PASS_COUNT);

	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::OBJECT)] = std::make_unique<CConstantBuffer>();
	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::OBJECT)]->Initialize(1, ALIGNED_SIZE(sizeof(CBObjectData)) * OBJECT_COUNT);

	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::LIGHT)] = std::make_unique<CConstantBuffer>();
	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::LIGHT)]->Initialize(2, ALIGNED_SIZE(sizeof(CBLightsData))*LIGHT_COUNT);

	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::BONE_TRANSFORM)] = std::make_unique<CConstantBuffer>();
	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::BONE_TRANSFORM)]->Initialize(3, ALIGNED_SIZE(sizeof(Matrix) * SKINNED_ANIMATION_BONES) * BONE_TRANSFORM_COUNT);

	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::MATERIAL)] = std::make_unique<CConstantBuffer>();
	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::MATERIAL)]->Initialize(4, ALIGNED_SIZE(100) * 100);
}


std::shared_ptr<CStructedBuffer> CFrameResource::GetStructedBuffer(UINT type)
{
	return mStructedBuffers[type];
}

std::shared_ptr<CConstantBuffer> CFrameResource::GetConstantBuffer(UINT type)
{
	return mConstantBuffers[type];
}

std::shared_ptr<CInstancingBuffer> CFrameResource::GetInstancingBuffer(UINT type)
{
	return mInstancingBuffers[type];
}

void CFrameResource::BindToShader()
{
	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::LIGHT)]->BindToShader();
}



