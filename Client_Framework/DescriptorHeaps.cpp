#include "stdafx.h"
#include "DescriptorHeaps.h"
#include"DX12Manager.h"
#include"ResourceManager.h"
#include"Texture.h"

void CDescriptorHeaps::Initialize(UINT cbvNum, UINT srvNum, UINT cubeMapNum, UINT uavNum)
{
	InitSrvDescriptorHeap(cbvNum, srvNum,cubeMapNum, uavNum);
	InitDsvDescriptorHeap();
}


void CDescriptorHeaps::InitDsvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = DS_TYPE_COUNT;
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

void CDescriptorHeaps::CreateDSV(std::shared_ptr<CTexture> resource, DS_TYPE type)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = dsvStartHandle.cpuHandle;
	handle.ptr += (dsvDescriptorSize * static_cast<size_t>(type));

	auto dsvDesc = resource->GetDSVDesc();
	DEVICE->CreateDepthStencilView(resource->GetResource().Get(), &dsvDesc, handle);
}

void CDescriptorHeaps::CreateSRV(std::shared_ptr<CTexture> resource, UINT idx) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = srvStartHandle.cpuHandle;
	handle.ptr += (cbvSrvDescriptorSize * static_cast<size_t>(idx));

	auto srvDesc = resource->GetSRVDesc();
	DEVICE->CreateShaderResourceView(resource->GetResource().Get(), &srvDesc, handle);
}

void CDescriptorHeaps::CreateCubeMap(std::shared_ptr<CTexture> resource, UINT idx) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = cubeMapStartHandle.cpuHandle;
	handle.ptr += (cbvSrvDescriptorSize * static_cast<size_t>(idx));

	auto srvDesc = resource->GetSRVDesc();
	DEVICE->CreateShaderResourceView(resource->GetResource().Get(), &srvDesc, handle);
}

void CDescriptorHeaps::CreateUAV(ComPtr<ID3D12Resource> resource, ComPtr<ID3D12Resource> counterResource
	, D3D12_UNORDERED_ACCESS_VIEW_DESC desc, UINT idx) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = uavStartHandle.cpuHandle;
	handle.ptr += (cbvSrvDescriptorSize * static_cast<size_t>(idx));

	DEVICE->CreateUnorderedAccessView(resource.Get(), counterResource.Get(), &desc, handle);
}

void CDescriptorHeaps::SetSRVDescriptorHeap()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap.Get() };
	CMDLIST->SetDescriptorHeaps(1, descriptorHeaps);
	CMDLIST->SetGraphicsRootDescriptorTable(6, srvStartHandle.gpuHandle);
	CMDLIST->SetGraphicsRootDescriptorTable(7, cubeMapStartHandle.gpuHandle);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE CDescriptorHeaps::GetDSVHandle(DS_TYPE type) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = dsvStartHandle.cpuHandle;
	handle.ptr += (dsvDescriptorSize * static_cast<size_t>(type));
	return handle;
}
