#include "stdafx.h"
#include "InstancingBuffer.h"


CInstancingBuffer::~CInstancingBuffer()
{
	if (buffer != nullptr) {
		buffer->Unmap(0, NULL);
	}
	mappedData = nullptr;
}

void CInstancingBuffer::Initialize(UINT _rootParamIdx, UINT _dataSize, UINT instanceNum)
{
	rootParamIdx = _rootParamIdx;
	mMaxInstanceNum = instanceNum;
	dataSize = _dataSize;

	bufferSize = mMaxInstanceNum * dataSize;

	CreateBuffer();

	mInstancingBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	mInstancingBufferView.StrideInBytes = dataSize;
	mInstancingBufferView.SizeInBytes = dataSize * mMaxInstanceNum;
}

void CInstancingBuffer::UpdateBuffer(UINT idx, const void* data)
{
	memcpy(&mappedData[idx * dataSize], data, dataSize);
}

void CInstancingBuffer::CreateBuffer()
{
	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ThrowIfFailed(DEVICE->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer)));

	D3D12_RANGE readRange = { 0, 0 };
	ThrowIfFailed(buffer->Map(0, &readRange, (void**)&mappedData));
}

void CInstancingBuffer::Resize(UINT maxInstanceNum)
{
	ComPtr<ID3D12Resource> newBuffer;

	UINT newBufferSize = maxInstanceNum * dataSize;

	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(newBufferSize);

	ThrowIfFailed(DEVICE->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&newBuffer)));

	BYTE* newMappedData{};
	D3D12_RANGE readRange = { 0, 0 };
	ThrowIfFailed(newBuffer->Map(0, &readRange, (void**)&newMappedData));

	buffer->Unmap(0, NULL);
	mappedData = newMappedData;
	buffer = newBuffer;

	bufferSize = newBufferSize;

	mInstancingBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	mInstancingBufferView.StrideInBytes = dataSize;
	mInstancingBufferView.SizeInBytes = bufferSize;
}
