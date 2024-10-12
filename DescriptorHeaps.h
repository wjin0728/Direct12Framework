#pragma once
#include"stdafx.h"

enum {
	SWAP_CHAIN_COUNT = 2,
	SRV_COUNT = 10,
	UAV_COUNT = 0
};

struct DescriptorHandle
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
};


class CDescriptorHeaps
{
private:
	friend class CDX12Manager;

	ComPtr<ID3D12DescriptorHeap> dsvHeap{};
	UINT dsvDescriptorSize{};

	ComPtr<ID3D12DescriptorHeap> srvHeap{};
	UINT cbvSrvDescriptorSize{};

	DescriptorHandle srvDescriptorHandle{};
	DescriptorHandle uavDescriptorHandle{};

public:
	void Initialize();

	void InitDsvDescriptorHeap();
	void InitSrvDescriptorHeap();

public:
	void CreateSRV(ComPtr<ID3D12Resource> resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc, UINT idx) const;
	void CreateUAV(ComPtr<ID3D12Resource> resource, ComPtr<ID3D12Resource> counterResource, 
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc, UINT idx) const;

};

