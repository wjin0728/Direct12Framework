#include "stdafx.h"
#include"DX12Manager.h"
#include"FrameResource.h"
#include"ResourceManager.h"
#include"InstancingBuffer.h"
#include"Material.h"

CFrameResource::CFrameResource()
{
	ThrowIfFailed(DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)));

	mLocalUploadBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::PASS)] = std::make_unique<CConstantBuffer>();
	mLocalUploadBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::PASS)]->Initialize(0, sizeof(CBPassData), PASS_COUNT);

	mLocalUploadBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::OBJECT)] = std::make_unique<CConstantBuffer>();
	mLocalUploadBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::OBJECT)]->Initialize(1, sizeof(CBObjectData), OBJECT_COUNT);

	mLocalUploadBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::LIGHT)] = std::make_unique<CConstantBuffer>();
	mLocalUploadBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::LIGHT)]->Initialize(2, sizeof(CBLightsData));

	mLocalUploadBuffers[static_cast<UINT>(STRUCTED_BUFFER_TYPE::MATERIAL)] = std::make_unique<CStructedBuffer>();
	mLocalUploadBuffers[static_cast<UINT>(STRUCTED_BUFFER_TYPE::MATERIAL)]->Initialize(3, sizeof(CBMaterialDate), MATERIAL_COUNT);

	mLocalUploadBuffers[static_cast<UINT>(INSTANCE_BUFFER_TYPE::BILLBOARD)] = std::make_unique<CInstancingBuffer>();
	mLocalUploadBuffers[static_cast<UINT>(INSTANCE_BUFFER_TYPE::BILLBOARD)]->Initialize(0, sizeof(BillboardData), BILLBOARD_COUNT);
}

std::shared_ptr<CUploadBuffer> CFrameResource::GetBuffer(UINT type)
{
	return mLocalUploadBuffers[type];
}

void CFrameResource::Update()
{
	mLocalUploadBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::PASS)]->UpdateBuffer();
	mLocalUploadBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::LIGHT)]->UpdateBuffer();
	mLocalUploadBuffers[static_cast<UINT>(STRUCTED_BUFFER_TYPE::MATERIAL)]->UpdateBuffer();
}



