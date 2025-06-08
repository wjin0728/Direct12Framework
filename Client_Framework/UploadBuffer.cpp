#include "stdafx.h"
#include "UploadBuffer.h"
#include"DX12Manager.h"


CStructedBuffer::~CStructedBuffer()
{
	if (buffer != nullptr) {
		buffer->Unmap(0, NULL);
	}
	mappedData = nullptr;
}

void CStructedBuffer::Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum, BYTE* initialData, bool writable)
{
	rootParamIdx = _rootParamIdx;
	dataSize = _dataSize;
	dataNum = _dataNum;
	bufferSize = dataSize * dataNum;

	mWritable = writable;

	CreateBuffer(initialData);
}

void CStructedBuffer::BindToShader()
{
	D3D12_GPU_VIRTUAL_ADDRESS bufferLocation = buffer->GetGPUVirtualAddress();

	auto cmdList = INSTANCE(CDX12Manager).GetCommandList();
	cmdList->SetGraphicsRootShaderResourceView(rootParamIdx, bufferLocation);
}

void CStructedBuffer::ReleaseUploadBuffer()
{
	if (uploadBuffer != nullptr) {
		uploadBuffer.Reset();
	}
}

void CStructedBuffer::UpdateBuffer(UINT idx, const void* _data)
{
	if(!mappedData) return;
	memcpy(&mappedData[idx* dataSize], _data, dataSize);
}

void CStructedBuffer::CreateBuffer(BYTE* initialData)
{
	if(mWritable)
	{
		CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		ThrowIfFailed(DEVICE->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
			&bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer)));

		D3D12_RANGE readRange = { 0, 0 };
		ThrowIfFailed(buffer->Map(0, &readRange, (void**)&mappedData));
	}
	else
	{
		buffer = CreateBufferResource(DEVICE, CMDLIST, initialData, bufferSize,
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, &uploadBuffer);
		mappedData = nullptr; // CPU 접근 불가
	}
}


CConstantBuffer::~CConstantBuffer()
{
	if (buffer != nullptr) {
		buffer->Unmap(0, NULL);
	}
	mappedData = nullptr;
}

void CConstantBuffer::Initialize(UINT _rootParamIdx, UINT initialSize)
{
	rootParamIdx = _rootParamIdx;
	bufferSize = ALIGNED_SIZE(initialSize);

	CreateBuffer();
}

UINT CConstantBuffer::AddData(const void* data, UINT dataSize)
{
	UINT currentOffset = mNextOffset;
	mNextOffset += ALIGNED_SIZE(dataSize);
	if (mNextOffset >= bufferSize) {
		Resize(mNextOffset * 1.5f);
	}
	memcpy(&mappedData[currentOffset], data, dataSize);

	return currentOffset;
}

void CConstantBuffer::UpdateBuffer(UINT offSet, const void* data, UINT dataSize)
{
	std::memcpy(&mappedData[offSet], data, dataSize);
}

void CConstantBuffer::BindToShader(UINT offSet)
{
	D3D12_GPU_VIRTUAL_ADDRESS bufferLocation = buffer->GetGPUVirtualAddress() + offSet;
	auto cmdList = INSTANCE(CDX12Manager).GetCommandList();
	cmdList->SetGraphicsRootConstantBufferView(rootParamIdx, bufferLocation);
}

void CConstantBuffer::CreateBuffer()
{
	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ThrowIfFailed(DEVICE->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer)));

	D3D12_RANGE readRange = { 0, 0 };
	ThrowIfFailed(buffer->Map(0, &readRange, (void**)&mappedData));
}

void CConstantBuffer::Resize(UINT size)
{
	ComPtr<ID3D12Resource> newBuffer;

	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	ThrowIfFailed(DEVICE->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&newBuffer)));

	BYTE* newMappedData{};
	D3D12_RANGE readRange = { 0, 0 };
	ThrowIfFailed(newBuffer->Map(0, &readRange, (void**)&newMappedData));

	std::memcpy(newMappedData, mappedData, bufferSize);

	buffer->Unmap(0, NULL);
	mappedData = newMappedData;
	buffer = newBuffer;

	bufferSize = size;
}