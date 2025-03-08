#include "stdafx.h"
#include"DX12Manager.h"
#include"FrameResource.h"
#include"ResourceManager.h"
#include"InstancingBuffer.h"
#include"Material.h"

CFrameResource::CFrameResource()
{
	ThrowIfFailed(DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)));

	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::PASS)] = std::make_unique<CConstantBuffer>();
	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::PASS)]->Initialize(0, sizeof(CBPassData) * PASS_COUNT);

	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::OBJECT)] = std::make_unique<CConstantBuffer>();
	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::OBJECT)]->Initialize(1, sizeof(CBObjectData) * OBJECT_COUNT);

	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::LIGHT)] = std::make_unique<CConstantBuffer>();
	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::LIGHT)]->Initialize(2, sizeof(CBLightsData));

	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::MATERIAL)] = std::make_unique<CConstantBuffer>();
	mConstantBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::MATERIAL)]->Initialize(3, 100 * MATERIAL_COUNT);

	mInstancingBuffers[static_cast<UINT>(INSTANCE_BUFFER_TYPE::BILLBOARD)] = std::make_unique<CInstancingBuffer>();
	mInstancingBuffers[static_cast<UINT>(INSTANCE_BUFFER_TYPE::BILLBOARD)]->Initialize(0, sizeof(BillboardData), BILLBOARD_COUNT);
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



