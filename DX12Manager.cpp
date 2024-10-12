#include "stdafx.h"
#include "DX12Manager.h"


void CDX12Manager::SetDevice()
{
	HRESULT hResult;
	UINT dXGIFactoryFlags = 0;

#if defined(_DEBUG)
	ComPtr<ID3D12Debug> d3dDebugController;
	hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController));
	if (d3dDebugController.Get())
	{
		d3dDebugController->EnableDebugLayer();
	}
	dXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

#endif

	ThrowIfFailed(CreateDXGIFactory2(dXGIFactoryFlags, IID_PPV_ARGS(&mFactory)));

	ComPtr<IDXGIAdapter1> d3dAdapter;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != mFactory->EnumAdapters1(i, &d3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		ThrowIfFailed(d3dAdapter->GetDesc1(&dxgiAdapterDesc));
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}
		if (SUCCEEDED(D3D12CreateDevice(d3dAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mDevice)))) {
			break;
		}
	}
	if (!d3dAdapter)
	{
		ThrowIfFailed(mFactory->EnumWarpAdapter(IID_PPV_ARGS(&d3dAdapter)));
		ThrowIfFailed(D3D12CreateDevice(d3dAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice)));
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(mDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)));
	msaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	msaa4xEnable = (msaa4xQualityLevels > 1) ? true : false;

	ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3dFence)));
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CDX12Manager::SetSwapChain(HWND hWnd)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	renderTargetSize.x = rcClient.right - rcClient.left;
	renderTargetSize.y = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc{};
	dxgiSwapChainDesc.BufferDesc.Width = renderTargetSize.x;
	dxgiSwapChainDesc.BufferDesc.Height = renderTargetSize.y;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.SampleDesc.Count = (msaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (msaa4xEnable) ? (msaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferCount = swapChainBufferNum;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = hWnd;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.Flags = 0;

	ComPtr<IDXGISwapChain> swapChain;
	ThrowIfFailed(mFactory->CreateSwapChain(cmdQueue.Get(),
		&dxgiSwapChainDesc, &swapChain));
	swapChain.As(&mSwapChain);

	swapChainBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	ThrowIfFailed(mFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	SetRenderTargetViews();
#endif
}

void CDX12Manager::SetCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(mDevice->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue)));
	ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc)));
	ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAlloc.Get(), NULL, IID_PPV_ARGS(&cmdList)));

	ThrowIfFailed(cmdList->Close());
}

void CDX12Manager::ChangeSwapChainState()
{
	WaitForGpu();

	BOOL bFullScreenState = FALSE;
	ThrowIfFailed(mSwapChain->GetFullscreenState(&bFullScreenState, NULL));
	ThrowIfFailed(mSwapChain->SetFullscreenState(!bFullScreenState, NULL));

	DXGI_MODE_DESC dxgiTargetParameters{};
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = renderTargetSize.y;
	dxgiTargetParameters.Height = renderTargetSize.y;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	/*ThrowIfFailed(mSwapChain->ResizeTarget(&dxgiTargetParameters));
	for (int i = 0; i < swapChainBufferNum; i++)
		if (d3dRenderTargetBuffers[i]) {
			d3dRenderTargetBuffers[i].Reset();
		}*/

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc{};
	ThrowIfFailed(mSwapChain->GetDesc(&dxgiSwapChainDesc));
	ThrowIfFailed(mSwapChain->ResizeBuffers(swapChainBufferNum, renderTargetSize.y,
		renderTargetSize.y, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags));
	swapChainBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	SetRenderTargetViews();
	//d3dDepthStencilBuffer.Reset();
	SetDepthStencilView();
}

void CDX12Manager::SetDescriptorHeaps()
{
	descriptorHeaps->Initialize();
}

void CDX12Manager::SetRenderTargetViews()
{
	
}

void CDX12Manager::SetDepthStencilView()
{
	CD3DX12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = renderTargetSize.y;
	d3dResourceDesc.Height = renderTargetSize.y;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (msaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (msaa4xEnable) ? (msaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	/*ThrowIfFailed(mDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, IID_PPV_ARGS(&d3dDepthStencilBuffer)));

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = descriptorHeaps->dsvHeap->GetCPUDescriptorHandleForHeapStart();

	mDevice->CreateDepthStencilView(d3dDepthStencilBuffer.Get(), NULL, d3dDsvCPUDescriptorHandle);*/
}

std::vector<CD3DX12_STATIC_SAMPLER_DESC> CDX12Manager::SetStaticSamplers()
{
	CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	CD3DX12_STATIC_SAMPLER_DESC anisotropicalWrap(
		4,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f,
		8
	);

	CD3DX12_STATIC_SAMPLER_DESC anisotropicalClamp(
		5,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0.0f,
		8
	);

	return std::vector<CD3DX12_STATIC_SAMPLER_DESC>{
		pointWrap, pointClamp, linearWrap, linearClamp, anisotropicalWrap, anisotropicalClamp};
}

void CDX12Manager::SetRootSignature()
{
	D3D12_DESCRIPTOR_RANGE textureTable;
	textureTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	textureTable.NumDescriptors = texNum;
	textureTable.BaseShaderRegister = 0; //t0: gtxtTexture
	textureTable.RegisterSpace = 0;
	textureTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[4];
	//오브젝트 정보
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0;
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//렌더 패스 정보
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1;
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//조명 정보
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2;
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//텍스쳐 정보
	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &textureTable;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;


	auto samplers = SetStaticSamplers();

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc{};
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = samplers.size();
	d3dRootSignatureDesc.pStaticSamplers = samplers.data();
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ComPtr<ID3DBlob> d3dSignatureBlob;
	ComPtr<ID3DBlob> d3dErrorBlob;

	HRESULT hr = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&d3dSignatureBlob, &d3dErrorBlob);

	if (d3dErrorBlob != nullptr) {
		OutputDebugStringA((char*)d3dErrorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(mDevice->CreateRootSignature(0, d3dSignatureBlob->GetBufferPointer(),
		d3dSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&d3dRootSignature)));
}

void CDX12Manager::Initialize(HWND hWnd)
{
	SetDevice();
	SetCommandQueueAndList();
	SetDescriptorHeaps();
	SetSwapChain(hWnd);
	SetDepthStencilView();

	CreateFrameResources(renderPassNum, objectNum);
}

void CDX12Manager::Destroy()
{
	WaitForGpu();

	CloseHandle(fenceEvent);

#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pdxgiDebug));
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
	mSwapChain->SetFullscreenState(FALSE, NULL);
}

void CDX12Manager::CreateFrameResources(UINT passNum, UINT objectNum)
{
	for (auto& frameResource : mFrameResources) {
		if (frameResource) {
			frameResource.reset();
		}
		frameResource = std::make_unique<CFrameResource>(passNum, objectNum, 10);
	}
}

void CDX12Manager::WaitForGpu()
{
	++mainFence;

	ThrowIfFailed(cmdQueue->Signal(d3dFence.Get(), mainFence));

	if (d3dFence->GetCompletedValue() < mainFence)
	{
		ThrowIfFailed(d3dFence->SetEventOnCompletion(mainFence, fenceEvent));
		::WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void CDX12Manager::OpenCommandList()
{
	ThrowIfFailed(cmdAlloc->Reset());
	ThrowIfFailed(cmdList->Reset(cmdAlloc.Get(), NULL));
}

void CDX12Manager::CloseCommandList()
{
	ThrowIfFailed(cmdList->Close());
	ID3D12CommandList* pcmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(pcmdLists), pcmdLists);

	WaitForGpu();
}

void CDX12Manager::MoveToNextFrameResource()
{
	curFrameResourceIdx = (curFrameResourceIdx + 1) % FRAME_RESOURCE_COUNT;
	mCurFrameResource = mFrameResources[curFrameResourceIdx].get();

	if (mCurFrameResource->fence != 0 && d3dFence->GetCompletedValue() < mCurFrameResource->fence)
	{
		ThrowIfFailed(d3dFence->SetEventOnCompletion(mCurFrameResource->fence, fenceEvent));
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}



void CDX12Manager::BeforeRender()
{
	auto& curFrameCmdAlloc = mCurFrameResource->cmdAllocator;

	ThrowIfFailed(curFrameCmdAlloc->Reset());
	ThrowIfFailed(cmdList->Reset(curFrameCmdAlloc.Get(), NULL));
}

void CDX12Manager::AfterRender()
{

	ThrowIfFailed(cmdList->Close());

	ID3D12CommandList* pcmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(pcmdLists), pcmdLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	swapChainBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	mCurFrameResource->fence = ++mainFence;

	ThrowIfFailed(cmdQueue->Signal(d3dFence.Get(), mainFence));
}

std::shared_ptr<CUploadBuffer> CDX12Manager::GetConstBuffer(CONSTANT_BUFFER_TYPE type)
{
	return mCurFrameResource->GetConstBuffer(type);
}
