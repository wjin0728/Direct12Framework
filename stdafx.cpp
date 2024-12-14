#include "stdafx.h"

ID3D12Resource* CreateBufferResource(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
	void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer)
{
	ID3D12Resource* pd3dBuffer = NULL;

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc = CD3DX12_HEAP_PROPERTIES(d3dHeapType);
	D3D12_RESOURCE_DESC d3dResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(nBytes);
	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	ThrowIfFailed(device->CreateCommittedResource(&d3dHeapPropertiesDesc,
		D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, nullptr,
		IID_PPV_ARGS(&pd3dBuffer)));

	if (pData)
	{
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppd3dUploadBuffer)
			{
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				ThrowIfFailed(device->CreateCommittedResource(&d3dHeapPropertiesDesc,
					D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
					__uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer));

				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8* pBufferDataBegin = NULL;

				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);

				memcpy(pBufferDataBegin, pData, nBytes);

				(*ppd3dUploadBuffer)->Unmap(0, NULL);

				cmdList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);

				D3D12_RESOURCE_BARRIER d3dResourceBarrier;
				::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));

				d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				d3dResourceBarrier.Transition.pResource = pd3dBuffer;
				d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
				d3dResourceBarrier.Transition.Subresource =
					D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				cmdList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pd3dBuffer);
}