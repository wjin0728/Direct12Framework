#pragma once
#include"stdafx.h"
#include"UploadBuffer.h"


enum {
	FRAME_RESOURCE_COUNT = 3
};


class CFrameResource
{
public: //��� �Ҵ���
	ComPtr<ID3D12CommandAllocator> cmdAllocator{};

private: //��� ����
	std::array<std::shared_ptr<CUploadBuffer>, CONSTANT_BUFFER_TYPE_COUNT> constBuffers{};

public: //�潺
	UINT64 fence{};

public:
	CFrameResource() {};
	CFrameResource(UINT passCnt, UINT objectCnt, UINT matCnt);
	~CFrameResource() {};

public:
	std::shared_ptr<CUploadBuffer> GetConstBuffer(CONSTANT_BUFFER_TYPE type);

};

