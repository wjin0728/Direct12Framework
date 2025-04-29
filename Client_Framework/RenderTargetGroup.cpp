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
		auto& resource = rt.rt->GetResource();

		device->CreateRenderTargetView(resource.Get(), NULL, handle);

		handle.ptr += rtvDescriptorSize;
	}

	//렌더타겟의 리소스 상태변환에 대한 배리어 생성
	targetToResource.reserve(rtNum);
	resourceToTarget.reserve(rtNum);

	for (auto& rt : renderTargets) {
		auto& resource = rt.rt->GetResource();

		targetToResource.push_back(
			CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON)
		);

		resourceToTarget.push_back(
			CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET)
		);
	}
}

void CRenderTargetGroup::SetRenderTargets()
{
	CMDLIST->OMSetRenderTargets(renderTargets.size(), &rtvHeapHandle, TRUE, &dsVHeapHandle);
}

void CRenderTargetGroup::SetRenderTarget(UINT idx)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(rtvHeapHandle, idx * rtvDescriptorSize);

	CMDLIST->OMSetRenderTargets(1, &handle, FALSE, &dsVHeapHandle);
}

void CRenderTargetGroup::SetOnlyDepthStencil()
{
	CMDLIST->OMSetRenderTargets(0, nullptr, FALSE, &dsVHeapHandle);
}

void CRenderTargetGroup::ClearRenderTargets()
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeapHandle;
	for (auto& rt : renderTargets) {
		CMDLIST->ClearRenderTargetView(handle, rt.clearColor, 0, NULL);

		handle.ptr += rtvDescriptorSize;
	}
}

void CRenderTargetGroup::ClearRenderTarget(UINT idx)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapHandle, (idx * rtvDescriptorSize));

	CMDLIST->ClearRenderTargetView(handle, renderTargets[idx].clearColor, 0, NULL);
}

void CRenderTargetGroup::ClearDepthStencil(float depth, UINT8 stencil)
{
	CMDLIST->ClearDepthStencilView(dsVHeapHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, NULL);
}

void CRenderTargetGroup::ClearOnlyStencil(UINT8 stencil)
{
	CMDLIST->ClearDepthStencilView(dsVHeapHandle, D3D12_CLEAR_FLAG_STENCIL, 0.f, stencil, 0, NULL);
}

void CRenderTargetGroup::ClearOnlyDepth(float depth)
{
	CMDLIST->ClearDepthStencilView(dsVHeapHandle, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, NULL);
}

void CRenderTargetGroup::ChangeTargetsToResources()
{
	CMDLIST->ResourceBarrier(targetToResource.size(), &targetToResource[0]);
}

void CRenderTargetGroup::ChangeTargetToResource(UINT idx)
{
	assert(targetToResource.size() > idx);

	CMDLIST->ResourceBarrier(1, &targetToResource[idx]);
}

void CRenderTargetGroup::ChangeResourcesToTargets()
{
	CMDLIST->ResourceBarrier(resourceToTarget.size(), &resourceToTarget[0]);
}

void CRenderTargetGroup::ChangeResourceToTarget(UINT idx)
{
	assert(resourceToTarget.size() > idx);

	CMDLIST->ResourceBarrier(1, &resourceToTarget[idx]);
}
