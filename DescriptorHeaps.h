#pragma once
#include"stdafx.h"


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

	DescriptorHandle dsvStartHandle{};
	DescriptorHandle srvStartHandle{};
	DescriptorHandle uavStartHandle{};

public:
	void Initialize(UINT cbvNum, UINT srvNum, UINT uavNum);

	void InitDsvDescriptorHeap();
	void InitSrvDescriptorHeap(UINT cbvNum, UINT srvNum, UINT uavNum);

	void CreateDSV(ComPtr<ID3D12Resource> resource);
	void CreateSRV(ComPtr<ID3D12Resource> resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc, UINT idx) const;
	void CreateUAV(ComPtr<ID3D12Resource> resource, ComPtr<ID3D12Resource> counterResource, 
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc, UINT idx) const;

	void SetSRVDescriptorHeap();
	void SetRootSignitureDescriptorTable();

	DescriptorHandle GetDSVStartHandle() const { return dsvStartHandle; }
	DescriptorHandle GetSRVStartHandle() const { return srvStartHandle; }
};

