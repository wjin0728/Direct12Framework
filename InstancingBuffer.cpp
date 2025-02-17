#include "stdafx.h"
#include "InstancingBuffer.h"


void CInstancingBuffer::Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum)
{
	rootParamIdx = _rootParamIdx;
	byteSize = dataSize = _dataSize;
	dataNum = _dataNum;

	CreateBuffer();

	mInstancingBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	mInstancingBufferView.StrideInBytes = byteSize;
	mInstancingBufferView.SizeInBytes = dataSize * dataNum;
}

void CInstancingBuffer::ReallocateBuffer(UINT maxInstanceNum)
{
	if (buffer != nullptr) {
		buffer->Unmap(0, NULL);
		buffer.Reset();
	}

	UINT dataSize = byteSize * maxInstanceNum;

	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);

	ThrowIfFailed(DEVICE->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer)));

	mMaxInstanceNum = maxInstanceNum;

	D3D12_RANGE readRange = { 0, 0 };
	ThrowIfFailed(buffer->Map(0, &readRange, (void**)&mappedData));

	mInstancingBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	mInstancingBufferView.StrideInBytes = byteSize;
	mInstancingBufferView.SizeInBytes = dataSize;
}
