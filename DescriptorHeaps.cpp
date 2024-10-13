#include "stdafx.h"
#include "DescriptorHeaps.h"
#include"DX12Manager.h"
#include"ResourceManager.h"

void CDescriptorHeaps::Initialize(UINT cbvNum, UINT srvNum, UINT uavNum)
{
	InitSrvDescriptorHeap(cbvNum, srvNum, uavNum);
	InitDsvDescriptorHeap();
}


void CDescriptorHeaps::InitDsvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	auto device = INSTANCE(CDX12Manager).GetDevice();
	ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));
	dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	dsvStartHandle.cpuHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void CDescriptorHeaps::InitSrvDescriptorHeap(UINT cbvNum, UINT srvNum, UINT uavNum)
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = cbvNum + srvNum + uavNum;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	auto device = DEVICE;
	ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));
	cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvStartHandle.cpuHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	srvStartHandle.gpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

	uavStartHandle.cpuHandle.ptr = (srvStartHandle.cpuHandle.ptr) + (cbvSrvDescriptorSize * static_cast<unsigned long long>(srvNum));
	uavStartHandle.gpuHandle.ptr = (srvStartHandle.gpuHandle.ptr) + (cbvSrvDescriptorSize * static_cast<unsigned long long>(srvNum));
}

void CDescriptorHeaps::CreateDSV(ComPtr<ID3D12Resource> resource)
{
	DEVICE->CreateDepthStencilView(resource.Get(), NULL, dsvStartHandle.cpuHandle);
}

void CDescriptorHeaps::CreateSRV(ComPtr<ID3D12Resource> resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc, UINT idx) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = srvStartHandle.cpuHandle;
	handle.ptr += (cbvSrvDescriptorSize * static_cast<unsigned long long>(idx));

	DEVICE->CreateShaderResourceView(resource.Get(), &desc, handle);
}

void CDescriptorHeaps::CreateUAV(ComPtr<ID3D12Resource> resource, ComPtr<ID3D12Resource> counterResource
	, D3D12_UNORDERED_ACCESS_VIEW_DESC desc, UINT idx) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = uavStartHandle.cpuHandle;
	handle.ptr += (cbvSrvDescriptorSize * idx);

	DEVICE->CreateUnorderedAccessView(resource.Get(), counterResource.Get(), & desc, handle);
}

void CDescriptorHeaps::SetSRVDescriptorHeap()
{
	CMDLIST->SetDescriptorHeaps(1, &srvHeap);
}
