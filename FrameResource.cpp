#include "stdafx.h"
#include"DX12Manager.h"
#include "FrameResource.h"
#include"Material.h"

CFrameResource::CFrameResource()
{
	ThrowIfFailed(DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)));

	for (auto& buffer : constBuffers) {
		buffer = std::make_unique<CUploadBuffer>();
	}
	
	constBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::PASS)]->Initialize(0, sizeof(CBPassData), PASS_COUNT);
	constBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::OBJECT)]->Initialize(1, sizeof(CBObjectData), OBJECT_COUNT);
	constBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::LIGHT)]->Initialize(2, sizeof(CBLightsData));
	constBuffers[static_cast<UINT>(CONSTANT_BUFFER_TYPE::MATERIAL)]->Initialize(3, sizeof(CMaterial), MATERIAL_COUNT);
}

std::shared_ptr<CUploadBuffer> CFrameResource::GetConstBuffer(CONSTANT_BUFFER_TYPE type)
{
	return constBuffers[static_cast<UINT>(type)];
}


