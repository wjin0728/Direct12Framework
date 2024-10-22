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

void CUploadBuffer::Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum)
{
	rootParamIdx = _rootParamIdx;
	dataSize = _dataSize;
	dataNum = _dataNum;

	CreateBuffer();
}

void CUploadBuffer::UpdateData(const void* _data, UINT idx, UINT _dataNum)
{
	assert(idx < dataNum);

	memcpy(&mappedData[idx * dataSize], _data, dataSize * static_cast<size_t>(_dataNum));
}

void CUploadBuffer::CreateBuffer()
{
	byteSize = ((sizeof(dataSize) + 255) & ~255);

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc{};
	d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc{};
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = byteSize * dataNum;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ThrowIfFailed(INSTANCE(CDX12Manager).GetDevice()->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&buffer)));

	D3D12_RANGE readRange = { 0, 0 };
	ThrowIfFailed(buffer->Map(0, &readRange, (void**)&mappedData));
	
}

void CUploadBuffer::UpdateConstantBuffer(UINT idx)
{
	D3D12_GPU_VIRTUAL_ADDRESS bufferLocation = buffer->GetGPUVirtualAddress() + (byteSize * static_cast<unsigned long long>(idx));

	auto cmdList = INSTANCE(CDX12Manager).GetCommandList();
	cmdList->SetGraphicsRootConstantBufferView(rootParamIdx, bufferLocation);
}

void CUploadBuffer::UpdateStructedBuffer(UINT idx)
{
	D3D12_GPU_VIRTUAL_ADDRESS bufferLocation = buffer->GetGPUVirtualAddress() + (byteSize * static_cast<unsigned long long>(idx));

	auto cmdList = INSTANCE(CDX12Manager).GetCommandList();
	cmdList->SetGraphicsRootShaderResourceView(rootParamIdx, bufferLocation);
}
