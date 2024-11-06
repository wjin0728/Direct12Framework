#pragma once
#include"stdafx.h"
#include"UploadBuffer.h"


enum {
	FRAME_RESOURCE_COUNT = 3
};


class CFrameResource
{
public: //명령 할당자
	ComPtr<ID3D12CommandAllocator> cmdAllocator = ComPtr<ID3D12CommandAllocator>();

private: //상수 버퍼
	std::array<std::shared_ptr<CUploadBuffer>, UPLOAD_BUFFER_TYPE_COUNT> mLocalUploadBuffers{};

public: //펜스
	UINT64 fence{};

public:
	CFrameResource();
	~CFrameResource() {};

public:
	std::shared_ptr<CUploadBuffer> GetBuffer(UINT type);
	void Update();
};

