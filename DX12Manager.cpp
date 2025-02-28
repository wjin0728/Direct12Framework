#include "stdafx.h"
#include "DX12Manager.h"
#include"ResourceManager.h"


void CDX12Manager::InitDevice()
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

void CDX12Manager::InitSwapChain(HWND hWnd)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	renderTargetSize.x = static_cast<UINT>(rcClient.right - rcClient.left);
	renderTargetSize.y = static_cast<UINT>(rcClient.bottom - rcClient.top);

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc{};
	dxgiSwapChainDesc.BufferDesc.Width = static_cast<UINT>(renderTargetSize.x);
	dxgiSwapChainDesc.BufferDesc.Height = static_cast<UINT>(renderTargetSize.y);
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.SampleDesc.Count = (msaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (msaa4xEnable) ? (msaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferCount = SWAP_CHAIN_COUNT;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = hWnd;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.Flags = 0;

	ComPtr<IDXGISwapChain> swapChain;
	ThrowIfFailed(mFactory->CreateSwapChain(cmdQueue.Get(),
		&dxgiSwapChainDesc, &swapChain));
	swapChain.As(&mSwapChain);

	curBackBuffIdx = mSwapChain->GetCurrentBackBufferIndex();
	ThrowIfFailed(mFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

}

void CDX12Manager::InitCommandQueueAndList()
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
	dxgiTargetParameters.RefreshRate.Numerator = 144;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	ThrowIfFailed(mSwapChain->ResizeTarget(&dxgiTargetParameters));

	for (int i = 0; i < SWAP_CHAIN_COUNT; i++) {
		auto target = INSTANCE(CResourceManager).Get<CTexture>(L"SwapChainTarget_" + std::to_wstring(i));
	}

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc{};
	ThrowIfFailed(mSwapChain->GetDesc(&dxgiSwapChainDesc));
	ThrowIfFailed(mSwapChain->ResizeBuffers(swapChainBufferNum, renderTargetSize.y,
		renderTargetSize.y, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags));
	curBackBuffIdx = mSwapChain->GetCurrentBackBufferIndex();

	InitDepthStencilView();
}

void CDX12Manager::InitDescriptorHeaps()
{
	descriptorHeaps = std::make_shared<CDescriptorHeaps>();
	descriptorHeaps->Initialize(0, TEXTURE_COUNT, CUBE_MAP_COUNT,0);
}

void CDX12Manager::InitRenderTargetGroups()
{
	auto dsvHeapHandle = descriptorHeaps->GetDSVHandle(DS_TYPE::MAIN_BUFFER);

#pragma region swapchain targets

	{
		std::vector<RenderTarget> renderTargets(SWAP_CHAIN_COUNT);

		for (UINT i = 0; i < renderTargets.size(); i++)
		{
			renderTargets[i].rt = std::make_shared<CTexture>();
			renderTargets[i].rt->SetName(L"SwapChainTarget_" + std::to_wstring(i));

			mSwapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i].rt->GetResource()));
			INSTANCE(CResourceManager).Add(renderTargets[i].rt);
		}

		renderTargetGroups[static_cast<UINT>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = std::make_shared<CRenderTargetGroup>();
		renderTargetGroups[static_cast<UINT>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Initialize(renderTargets, dsvHeapHandle);
	}
#pragma endregion

#pragma region Shadow Pass

	{
		auto dsv = descriptorHeaps->GetDSVHandle(DS_TYPE::SHADOW_MAP);
		std::vector<RenderTarget> renderTargets;


		renderTargetGroups[static_cast<UINT>(RENDER_TARGET_GROUP_TYPE::SHADOW_PASS)] = std::make_shared<CRenderTargetGroup>();
		renderTargetGroups[static_cast<UINT>(RENDER_TARGET_GROUP_TYPE::SHADOW_PASS)]->Initialize(renderTargets, dsv);
	}
#pragma endregion
}

void CDX12Manager::InitDepthStencilView()
{
	auto dsBuffer = INSTANCE(CResourceManager).Create2DTexture
	(
		L"DepthStencil", 
		DXGI_FORMAT_R24G8_TYPELESS,
		nullptr, 0,
		static_cast<UINT>(renderTargetSize.x), static_cast<UINT>(renderTargetSize.y),
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, 
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	dsBuffer->SetTextureType(DEPTH_STENCIL);
	descriptorHeaps->CreateDSV(dsBuffer, DS_TYPE::MAIN_BUFFER);

	auto shadowMap = INSTANCE(CResourceManager).Create2DTexture
	(
		L"ShadowMap",
		DXGI_FORMAT_R24G8_TYPELESS,
		nullptr, 0,
		4048, 4048,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	shadowMap->SetTextureType(DEPTH_STENCIL);
	descriptorHeaps->CreateDSV(shadowMap, DS_TYPE::SHADOW_MAP);
}

std::vector<CD3DX12_STATIC_SAMPLER_DESC> CDX12Manager::InitStaticSamplers()
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

	CD3DX12_STATIC_SAMPLER_DESC shadow(
		6, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	return std::vector<CD3DX12_STATIC_SAMPLER_DESC>{
		pointWrap, pointClamp, linearWrap, linearClamp, anisotropicalWrap, anisotropicalClamp, shadow};
}

void CDX12Manager::InitRootSignature()
{
	D3D12_DESCRIPTOR_RANGE textureTable{};
	textureTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	textureTable.NumDescriptors = TEXTURE_COUNT;
	textureTable.BaseShaderRegister = 0; //t0: 
	textureTable.RegisterSpace = 0;
	textureTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE cubeMapTable{};
	cubeMapTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	cubeMapTable.NumDescriptors = CUBE_MAP_COUNT;
	cubeMapTable.BaseShaderRegister = 0; 
	cubeMapTable.RegisterSpace = 2;
	cubeMapTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE shadowMapTable{};
	shadowMapTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	shadowMapTable.NumDescriptors = 1;
	shadowMapTable.BaseShaderRegister = 0;
	shadowMapTable.RegisterSpace = 3;
	shadowMapTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[7];
	//렌더 패스 정보
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0;
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//오브젝트 정보
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1;
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//조명 정보
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2;
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//재질 정보
	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 0;
	pd3dRootParameters[3].Descriptor.RegisterSpace = 1;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//텍스쳐 정보
	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &textureTable;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//큐브맵 정보
	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &cubeMapTable;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &shadowMapTable;
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	auto samplers = InitStaticSamplers();

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

	ThrowIfFailed(mDevice->CreateRootSignature(0, d3dSignatureBlob->GetBufferPointer(),
		d3dSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));
}

void CDX12Manager::Initialize(HWND hWnd)
{
	InitDevice();
	InitCommandQueueAndList();
	InitSwapChain(hWnd);
	InitDescriptorHeaps();
	InitDepthStencilView();
	InitRootSignature();
	InitRenderTargetGroups();
	CreateFrameResources();
}

void CDX12Manager::Destroy()
{
	WaitForGpu();

	CloseHandle(fenceEvent);

#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pdxgiDebug));
	/*HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);*/
	pdxgiDebug->Release();
#endif
	mSwapChain->SetFullscreenState(FALSE, NULL);
}

void CDX12Manager::CreateFrameResources()
{
	for (auto& frameResource : mFrameResources) {
		if (frameResource) {
			frameResource.reset();
		}
		frameResource = std::make_unique<CFrameResource>();
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
	ThrowIfFailed(cmdList->Reset(cmdAlloc.Get(), NULL));
}

void CDX12Manager::CloseCommandList()
{
	ThrowIfFailed(cmdList->Close());
	ID3D12CommandList* pcmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(pcmdLists), pcmdLists);

	WaitForGpu();
	ThrowIfFailed(cmdAlloc->Reset());
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

void CDX12Manager::UpdateFrameResources()
{
	mCurFrameResource->Update();
}

void CDX12Manager::BeforeRender()
{
	auto& curFrameCmdAlloc = mCurFrameResource->cmdAllocator;

	ThrowIfFailed(curFrameCmdAlloc->Reset());
	ThrowIfFailed(cmdList->Reset(curFrameCmdAlloc.Get(), NULL));

	cmdList->SetGraphicsRootSignature(mRootSignature.Get());
	descriptorHeaps->SetSRVDescriptorHeap();
	mCurFrameResource->Update();
}

void CDX12Manager::AfterRender()
{
	ThrowIfFailed(cmdList->Close());

	ID3D12CommandList* pcmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(pcmdLists), pcmdLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	curBackBuffIdx = mSwapChain->GetCurrentBackBufferIndex();

	mCurFrameResource->fence = ++mainFence;

	ThrowIfFailed(cmdQueue->Signal(d3dFence.Get(), mainFence));
}

void CDX12Manager::PrepareShadowPass()
{
	auto& swapChainBuffers = renderTargetGroups[(UINT)RENDER_TARGET_GROUP_TYPE::SHADOW_PASS];
	auto shadowMap = RESOURCE.Get<CTexture>(L"ShadowMap");

	shadowMap->ChangeResourceState(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	swapChainBuffers->SetOnlyDepthStencil();
	swapChainBuffers->ClearDepthStencil(1.f);
}

void CDX12Manager::PrepareFinalPass()
{
	auto& swapChainBuffers = renderTargetGroups[(UINT)RENDER_TARGET_GROUP_TYPE::FINAL_PASS];
	swapChainBuffers->ChangeResourceToTarget(0);
	swapChainBuffers->SetRenderTarget(0);
	swapChainBuffers->ClearRenderTarget(0);
}

std::shared_ptr<CUploadBuffer> CDX12Manager::GetBuffer(UINT type)
{
	return mCurFrameResource->GetBuffer(type);
}


