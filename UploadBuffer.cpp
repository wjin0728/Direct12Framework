#include "stdafx.h"
#include "UploadBuffer.h"
#include"DX12Manager.h"


CUploadBuffer::~CUploadBuffer()
{
	if (buffer != nullptr) {
		buffer->Unmap(0, NULL);
	}
	mappedData = nullptr;
}

void CUploadBuffer::CopyData(const void* _data, UINT idx)
{
	assert(idx < dataNum);

	memcpy(&mappedData[idx * byteSize], _data, dataSize);
}

void CUploadBuffer::CreateBuffer()
{
	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize* dataNum);

	ThrowIfFailed(DEVICE->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, 
		&bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer)));

	D3D12_RANGE readRange = { 0, 0 };
	ThrowIfFailed(buffer->Map(0, &readRange, (void**)&mappedData));
}


void CConstantBuffer::Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum)
{
	rootParamIdx = _rootParamIdx;
	dataSize = _dataSize;
	dataNum = _dataNum;
	byteSize = ((dataSize + 255) & ~255);

	CreateBuffer();
}

void CConstantBuffer::UpdateBuffer(UINT idx)
{
	D3D12_GPU_VIRTUAL_ADDRESS bufferLocation = buffer->GetGPUVirtualAddress() + (byteSize * static_cast<size_t>(idx));

	auto cmdList = INSTANCE(CDX12Manager).GetCommandList();
	cmdList->SetGraphicsRootConstantBufferView(rootParamIdx, bufferLocation);
}

void CStructedBuffer::Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum)
{
	rootParamIdx = _rootParamIdx;
	dataSize = _dataSize;
	dataNum = _dataNum;
	byteSize = dataSize;

	CreateBuffer();
}

void CStructedBuffer::UpdateBuffer(UINT idx)
{
	D3D12_GPU_VIRTUAL_ADDRESS bufferLocation = buffer->GetGPUVirtualAddress() + (byteSize * static_cast<size_t>(idx));

	auto cmdList = INSTANCE(CDX12Manager).GetCommandList();
	cmdList->SetGraphicsRootShaderResourceView(rootParamIdx, bufferLocation);
}
