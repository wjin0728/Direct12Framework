#pragma once
#include"FrameResource.h"




class CFrameResourceManager
{
	MAKE_SINGLETON(CFrameResourceManager)

private:
	std::array<std::unique_ptr<CFrameResource>, FRAME_RESOURCE_COUNT> mFrameResources{};
	CFrameResource* mCurFrameResource{};
	UINT curFrameResourceIdx{};

	std::priority_queue<UINT, std::vector<UINT>, std::greater<UINT>> cbvIdxQueue;

public:
	void Initialize();

public:
	void MoveToNextFrameResource();
	CFrameResource* GetCurFrameResource() const { return mCurFrameResource; }
	std::shared_ptr<CUploadBuffer> GetConstBuffer(CONSTANT_BUFFER_TYPE type);

private:
	void CreateFrameResources();

	UINT GetTopCBVIndex();

};

