#include "stdafx.h"
#include "FrameResourceManager.h"

void CFrameResourceManager::Initialize()
{
	CreateFrameResources();
}

void CFrameResourceManager::MoveToNextFrameResource()
{
	curFrameResourceIdx = (curFrameResourceIdx + 1) % FRAME_RESOURCE_COUNT;
	mCurFrameResource = mFrameResources[curFrameResourceIdx].get();

	if (mCurFrameResource->fence != 0 && d3dFence->GetCompletedValue() < mCurFrameResource->fence)
	{
		ThrowIfFailed(d3dFence->SetEventOnCompletion(mCurFrameResource->fence, fenceEvent));
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void CFrameResourceManager::CreateFrameResources()
{
	for (auto& frameResource : mFrameResources) {
		if (frameResource) {
			frameResource.reset();
		}
		frameResource = std::make_unique<CFrameResource>();
	}
}

std::shared_ptr<CUploadBuffer> CFrameResourceManager::GetConstBuffer(CONSTANT_BUFFER_TYPE type)
{
	return mCurFrameResource->GetConstBuffer(type);
}

UINT CFrameResourceManager::GetTopCBVIndex()
{
	UINT idx = cbvIdxQueue.top();
	cbvIdxQueue.pop();

	return idx;
}
