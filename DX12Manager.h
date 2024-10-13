#pragma once
#include"stdafx.h"
#include"FrameResource.h"
#include"DescriptorHeaps.h"
#include"RenderTargetGroup.h"
#include"Texture.h"


class CDX12Manager
{
	MAKE_SINGLETON(CDX12Manager)

public:
	static const UINT swapChainBufferNum  = 2;
	static const UINT rtvNum			  = 2;
	static const UINT srvNum			  = 100;
	static const UINT uavNum			  = 0;
	static const UINT frameResourceNum	  = 3;
	static const UINT meshNum			  = 100;
	static const UINT texNum			  = 100;
	static const UINT objectNum			  = 100;
	static const UINT renderPassNum		  = 1;

private:
	//팩토리
	ComPtr<IDXGIFactory4> mFactory{};
	//디바이스
	ComPtr<ID3D12Device> mDevice{};

	//스왑체인
	ComPtr<IDXGISwapChain3> mSwapChain{};
	UINT curBackBuffIdx{};
	bool msaa4xEnable = true;
	UINT msaa4xQualityLevels{};

	//커맨드
	ComPtr<ID3D12CommandQueue> cmdQueue{};
	ComPtr<ID3D12CommandAllocator> cmdAlloc{};
	ComPtr<ID3D12GraphicsCommandList> cmdList{};

	//렌더 타겟 그룹
	std::array<std::shared_ptr<CRenderTargetGroup>, RENDER_TARGET_GROUP_TYPE_COUNT> renderTargetGroups;

	//DSV, SRV 디스크립터 힙
	std::shared_ptr<CDescriptorHeaps> descriptorHeaps;

	//프레임 리소스
	std::array<std::unique_ptr<CFrameResource>, FRAME_RESOURCE_COUNT> mFrameResources{};
	CFrameResource* mCurFrameResource{};
	int curFrameResourceIdx{};
	//펜스
	ComPtr<ID3D12Fence> d3dFence{};
	UINT64 mainFence{};
	HANDLE fenceEvent{};

	//루트 시그니처
	ComPtr<ID3D12RootSignature> mRootSignature{};
	
	XMFLOAT2 renderTargetSize{};

private:
	void InitDevice();
	void InitCommandQueueAndList();
	void InitSwapChain(HWND hWnd);
	void InitDescriptorHeaps();
	void InitDepthStencilView();
	void InitRenderTargetGroups();
	std::vector<CD3DX12_STATIC_SAMPLER_DESC> InitStaticSamplers();
	void InitRootSignature();
	void CreateFrameResources();

public:
	void Initialize(HWND hWnd);
	void Destroy();
	
	void ChangeSwapChainState();

	void MoveToNextFrameResource();
	void WaitForGpu();

	void OpenCommandList();
	void CloseCommandList();

	void BeforeRender();
	void AfterRender();

public:
	ID3D12GraphicsCommandList* GetCommandList() const { return cmdList.Get(); }
	ID3D12Device* GetDevice() const { return mDevice.Get(); }
	ID3D12RootSignature* GetRootSignature() const {	return mRootSignature.Get(); }
	CFrameResource* GetCurFrameResource() const { return mCurFrameResource; }
	std::shared_ptr<CUploadBuffer> GetConstBuffer(CONSTANT_BUFFER_TYPE type);
	std::shared_ptr<CDescriptorHeaps> GetDescriptorHeaps() const { return descriptorHeaps; };

	XMFLOAT2 GetRenderTargetSize() const { return renderTargetSize; }
};

