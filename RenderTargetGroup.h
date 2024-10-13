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
	float clearColor[4] = {1.f, 1.f, 1.f, 1.f};
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
	std::vector<D3D12_RESOURCE_BARRIER> resourceToTarget{};

public:
	CRenderTargetGroup() {}
	~CRenderTargetGroup() {}

public:
	void Initialize(std::vector<RenderTarget>& rtVec, D3D12_CPU_DESCRIPTOR_HANDLE _dsVHeapHandle);

	void BeforeRender();

	void SetRenderTargets();
	void SetRenderTarget(UINT idx);

	void ClearRenderTargets();
	void ClearRenderTarget(UINT idx);
	void ClearDepthStencil(float depth = 1.0f, UINT8 stencil = 0);


	void ChangeTargetsToResources();
	void ChangeTargetToResource(UINT idx);

	void ChangeResourcesToTargets();
	void ChangeResourceToTarget(UINT idx);
};

