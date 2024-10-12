#pragma once
#include"stdafx.h"
#include"Texture.h"

enum class RENDER_TARGET_GROUP_TYPE : UINT
{
	SWAP_CHAIN,
	G_BUFFER,

	END
};

enum {
	RENDER_TARGET_GROUP_TYPE_COUNT = RENDER_TARGET_GROUP_TYPE::END,

	SWAP_CHAIN_COUNT = 2
};

struct RenderTarget
{
	std::shared_ptr<CTexture> rt;
	float clearColor[4];
};

class CRenderTargetGroup
{
private:
	ComPtr<ID3D12DescriptorHeap> rtvHeap{};
	std::vector<RenderTarget> renderTargets{};

	UINT rtvDescriptorSize{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle{};
	D3D12_CPU_DESCRIPTOR_HANDLE dsVHeapHandle{};

	std::vector<D3D12_RESOURCE_BARRIER> targetToResource{};
	std::vector<D3D12_RESOURCE_BARRIER> ResourceToTarget{};

public:
	CRenderTargetGroup() {}
	~CRenderTargetGroup() {}

public:
	void Initialize(std::vector<RenderTarget>& rtVec, D3D12_CPU_DESCRIPTOR_HANDLE _dsVHeapHandle);

	void BeforeRender();
	void AfterRender();

};

