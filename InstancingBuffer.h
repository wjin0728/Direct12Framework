#pragma once
#include"DX12Manager.h"
#include"UploadBuffer.h"

class CInstancingBuffer : public CUploadBuffer
{
private:
	D3D12_VERTEX_BUFFER_VIEW mInstancingBufferView{};

	UINT mMaxInstanceNum{};

public:
	CInstancingBuffer() = default;
	~CInstancingBuffer() {};

public:
	virtual void Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum = 1);

public:
	D3D12_VERTEX_BUFFER_VIEW GetInstancingBufferView() const { return mInstancingBufferView; }
	UINT GetMaxInstanceNum() const { return mMaxInstanceNum; }

private:
	void ReallocateBuffer(UINT maxInstanceNum);
};
