#pragma once
#include"stdafx.h"

enum class DS_TYPE : UINT
{
	MAIN_BUFFER,
	SHADOW_MAP,

	END
};

enum {
	DS_TYPE_COUNT = DS_TYPE::END
};

struct DescriptorHandle
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
};

class CTexture;

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
	DescriptorHandle cubeMapStartHandle{};
	DescriptorHandle uavStartHandle{};

	DescriptorHandle shadowMapHandle{};

public:
	void Initialize(UINT cbvNum, UINT srvNum, UINT cubeMapNum, UINT uavNum);

	void InitDsvDescriptorHeap();
	void InitSrvDescriptorHeap(UINT cbvNum, UINT srvNum, UINT cubeMapNum, UINT uavNum);

	void CreateDSV(std::shared_ptr<CTexture> resource, DS_TYPE type);
	void CreateSRV(std::shared_ptr<CTexture> resource, UINT idx) const;
	void CreateCubeMap(std::shared_ptr<CTexture> resource, UINT idx) const;
	void CreateUAV(ComPtr<ID3D12Resource> resource, ComPtr<ID3D12Resource> counterResource,
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc, UINT idx) const;
	void SetSRVDescriptorHeap();

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(DS_TYPE type) const;
	DescriptorHandle GetSRVStartHandle() const { return srvStartHandle; }
};

