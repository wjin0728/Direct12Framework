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
	std::array<std::shared_ptr<CUploadBuffer>, UPLOAD_BUFFER_TYPE_COUNT> mLocalUploadBuffers{};

public: //�潺
	UINT64 fence{};

public:
	CFrameResource();
	~CFrameResource() {};

public:
	std::shared_ptr<CUploadBuffer> GetBuffer(UINT type);
	void Update();
};

