#pragma once
#include"stdafx.h"
#include"UploadBuffer.h"


enum {
	FRAME_RESOURCE_COUNT = 3
};


class CFrameResource
{
public: //��� �Ҵ���
	ComPtr<ID3D12CommandAllocator> cmdAllocator = ComPtr<ID3D12CommandAllocator>();

private: //��� ����
	std::array<std::shared_ptr<CUploadBuffer>, CONSTANT_BUFFER_TYPE_COUNT> constBuffers{};

public: //�潺
	UINT64 fence{};
	std::priority_queue<UINT, std::vector<UINT>, std::greater<UINT>> cbvIdxQueue;

public:
	CFrameResource();
	~CFrameResource() {};

public:
	std::shared_ptr<CUploadBuffer> GetConstBuffer(CONSTANT_BUFFER_TYPE type);
	UINT GetTopSRVIndex();

	void ReturnSRVIndex(UINT idx) { cbvIdxQueue.push(idx); }
};

