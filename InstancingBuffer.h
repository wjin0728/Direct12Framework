#pragma once
#include"DX12Manager.h"
#include"UploadBuffer.h"

class CInstancingBuffer
{
private:
	D3D12_VERTEX_BUFFER_VIEW mInstancingBufferView{};

	ComPtr<ID3D12Resource> buffer{};

	BYTE* mappedData{};
	UINT rootParamIdx{};

	UINT dataSize{};
	UINT bufferSize{};
	UINT mMaxInstanceNum{};

public:
	CInstancingBuffer() = default;
	~CInstancingBuffer();

public:
	virtual void Initialize(UINT _rootParamIdx, UINT _dataSize, UINT instanceNum = 1);
	void UpdateBuffer(UINT idx, const void* data);

public:
	D3D12_VERTEX_BUFFER_VIEW GetInstancingBufferView() const { return mInstancingBufferView; }
	UINT GetMaxInstanceNum() const { return mMaxInstanceNum; }

private:
	void CreateBuffer();
	void Resize(UINT maxInstanceNum);
};
