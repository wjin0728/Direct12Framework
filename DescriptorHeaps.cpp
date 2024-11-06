#include "stdafx.h"
#include "DescriptorHeaps.h"
#include"DX12Manager.h"
#include"ResourceManager.h"

void CDescriptorHeaps::Initialize(UINT cbvNum, UINT srvNum, UINT cubeMapNum, UINT uavNum)
{
	InitSrvDescriptorHeap(cbvNum, srvNum,cubeMapNum, uavNum);
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

void CDescriptorHeaps::InitSrvDescriptorHeap(UINT cbvNum, UINT srvNum, UINT cubeMapNum, UINT uavNum)
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = cbvNum + srvNum + cubeMapNum + uavNum;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	auto device = DEVICE;
	ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));
	cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvStartHandle.cpuHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	srvStartHandle.gpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

	cubeMapStartHandle.cpuHandle.ptr = (srvStartHandle.cpuHandle.ptr) + (cbvSrvDescriptorSize * static_cast<size_t>(srvNum));
	cubeMapStartHandle.gpuHandle.ptr = (srvStartHandle.gpuHandle.ptr) + (cbvSrvDescriptorSize * static_cast<size_t>(srvNum));

	uavStartHandle.cpuHandle.ptr = (cubeMapStartHandle.cpuHandle.ptr) + (cbvSrvDescriptorSize * static_cast<size_t>(cubeMapNum));
	uavStartHandle.gpuHandle.ptr = (cubeMapStartHandle.gpuHandle.ptr) + (cbvSrvDescriptorSize * static_cast<size_t>(cubeMapNum));
}

void CDescriptorHeaps::CreateDSV(ComPtr<ID3D12Resource> resource)
{
	DEVICE->CreateDepthStencilView(resource.Get(), NULL, dsvStartHandle.cpuHandle);
}

void CDescriptorHeaps::CreateSRV(ComPtr<ID3D12Resource> resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc, UINT idx) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = srvStartHandle.cpuHandle;
	handle.ptr += (cbvSrvDescriptorSize * static_cast<size_t>(idx));

	DEVICE->CreateShaderResourceView(resource.Get(), &desc, handle);
}

void CDescriptorHeaps::CreateCubeMap(ComPtr<ID3D12Resource> resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc, UINT idx) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = cubeMapStartHandle.cpuHandle;
	handle.ptr += (cbvSrvDescriptorSize * static_cast<size_t>(idx));

	DEVICE->CreateShaderResourceView(resource.Get(), &desc, handle);
}

void CDescriptorHeaps::CreateUAV(ComPtr<ID3D12Resource> resource, ComPtr<ID3D12Resource> counterResource
	, D3D12_UNORDERED_ACCESS_VIEW_DESC desc, UINT idx) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = uavStartHandle.cpuHandle;
	handle.ptr += (cbvSrvDescriptorSize * static_cast<size_t>(idx));

	DEVICE->CreateUnorderedAccessView(resource.Get(), counterResource.Get(), & desc, handle);
}

void CDescriptorHeaps::SetSRVDescriptorHeap()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap.Get() };
	CMDLIST->SetDescriptorHeaps(1, descriptorHeaps);
	CMDLIST->SetGraphicsRootDescriptorTable(4, srvStartHandle.gpuHandle);
	CMDLIST->SetGraphicsRootDescriptorTable(5, cubeMapStartHandle.gpuHandle);
}
