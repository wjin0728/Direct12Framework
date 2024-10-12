#include "stdafx.h"
#include "RenderTargetGroup.h"
#include"DX12Manager.h"

void CRenderTargetGroup::Initialize(std::vector<RenderTarget>& rtVec, D3D12_CPU_DESCRIPTOR_HANDLE _dsVHeapHandle)
{
	renderTargets = rtVec;
	dsVHeapHandle = _dsVHeapHandle;

	UINT rtNum = renderTargets.size();

	//SRV 디스크립터 힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = rtNum;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;

	auto device = INSTANCE(CDX12Manager).GetDevice();
	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap));
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	rtvHeapHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

	//RTV 생성
	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeapHandle;

	for (auto& rt : renderTargets) {
		auto resource = rt.rt->GetResource();

		device->CreateRenderTargetView(resource.Get(), NULL, handle);

		handle.ptr += rtvDescriptorSize;
	}

	//렌더타겟의 리소스 상태변환에 대한 배리어 생성
	targetToResource.reserve(rtNum);
	ResourceToTarget.reserve(rtNum);

	for (auto& rt : renderTargets) {
		auto resource = rt.rt->GetResource();

		targetToResource.push_back(
			CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON)
		);

		ResourceToTarget.push_back(
			CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET)
		);
	}
}

void CRenderTargetGroup::BeforeRender()
{
	auto cmdList = INSTANCE(CDX12Manager).GetCommandList();

	cmdList->ResourceBarrier(ResourceToTarget.size(), ResourceToTarget.data());

	cmdList->OMSetRenderTargets(renderTargets.size(), &rtvHeapHandle, TRUE, &dsVHeapHandle);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeapHandle;
	for (auto& rt : renderTargets) {
		cmdList->ClearRenderTargetView(handle, rt.clearColor, 0, NULL);

		handle.ptr += rtvDescriptorSize;
	}

	cmdList->ClearDepthStencilView(dsVHeapHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
}

void CRenderTargetGroup::AfterRender()
{
	auto cmdList = INSTANCE(CDX12Manager).GetCommandList();

	cmdList->ResourceBarrier(targetToResource.size(), targetToResource.data());
}
