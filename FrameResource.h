#pragma once
#include"stdafx.h"
#include"UploadBuffer.h"


enum {
	FRAME_RESOURCE_COUNT = 1
};

class CInstancingBuffer;

class CFrameResource
{
public: //��� �Ҵ���
	ComPtr<ID3D12CommandAllocator> cmdAllocator = ComPtr<ID3D12CommandAllocator>();

private: //��� ����
	std::array<std::shared_ptr<CConstantBuffer>, (UINT)(CONSTANT_BUFFER_TYPE::END)> mConstantBuffers{};
	std::array<std::shared_ptr<CStructedBuffer>, (UINT)STRUCTED_BUFFER_TYPE::END> mStructedBuffers{};
	std::array<std::shared_ptr<CInstancingBuffer>, (UINT)INSTANCE_BUFFER_TYPE::END> mInstancingBuffers{};

public: //�潺
	UINT64 fence{};

public:
	CFrameResource();
	~CFrameResource() {};

public:
	std::shared_ptr<CStructedBuffer> GetStructedBuffer(UINT type);
	std::shared_ptr<CConstantBuffer> GetConstantBuffer(UINT type);
	std::shared_ptr<CInstancingBuffer> GetInstancingBuffer(UINT type);
	void BindToShader();
};

