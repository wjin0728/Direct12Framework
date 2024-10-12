#include "stdafx.h"
#include "DescriptorHeaps.h"
#include"DX12Manager.h"

void CDescriptorHeaps::Initialize()
{
	InitDsvDescriptorHeap();
	InitSrvDescriptorHeap();
}


void CDescriptorHeaps::InitDsvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = SWAP_CHAIN_COUNT;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	auto device = INSTANCE(CDX12Manager).GetDevice();
	ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));

	dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CDescriptorHeaps::InitSrvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = SRV_COUNT + UAV_COUNT;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	auto device = INSTANCE(CDX12Manager).GetDevice();
	ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));
	cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvDescriptorHandle.cpuHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	srvDescriptorHandle.gpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

	uavDescriptorHandle.cpuHandle.ptr = (srvDescriptorHandle.cpuHandle.ptr) + (cbvSrvDescriptorSize * static_cast<UINT>(SRV_COUNT));
	uavDescriptorHandle.gpuHandle.ptr = (srvDescriptorHandle.gpuHandle.ptr) + (cbvSrvDescriptorSize * static_cast<UINT>(SRV_COUNT));
}

void CDescriptorHeaps::CreateSRV(ComPtr<ID3D12Resource> resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc, UINT idx) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = srvDescriptorHandle.cpuHandle;
	handle.ptr += (cbvSrvDescriptorSize * idx);

	INSTANCE(CDX12Manager).GetDevice()->CreateShaderResourceView(resource.Get(), &desc, handle);
}

void CDescriptorHeaps::CreateUAV(ComPtr<ID3D12Resource> resource, ComPtr<ID3D12Resource> counterResource
	, D3D12_UNORDERED_ACCESS_VIEW_DESC desc, UINT idx) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = uavDescriptorHandle.cpuHandle;
	handle.ptr += (cbvSrvDescriptorSize * idx);

	INSTANCE(CDX12Manager).GetDevice()->CreateUnorderedAccessView(resource.Get(), counterResource.Get(), & desc, handle);
}
