#pragma once
#include"DX12Manager.h"

class CIndexBuffer
{
private:
	friend class CMesh;

	ComPtr<ID3D12Resource> mBuffer{};
	ComPtr<ID3D12Resource> mUploadBuffer{};
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView{};

	UINT mStartIndex{};
	int mBaseVertex{};
	UINT mIndicesNum{};

public:
	CIndexBuffer() = default;
	~CIndexBuffer() = default;

public:
	template<typename T>
	void CreateBuffer(const std::vector<T>& indices, UINT startIndex = 0);
	void ReleaseUploadBuffer();
	void ReleaseBuffer();

	void SetIndexBuffer() const;

public:
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const { return mIndexBufferView; }
};

template<typename T>
inline void CIndexBuffer::CreateBuffer(const std::vector<T>& indices, UINT startIndex)
{
	mStartIndex = startIndex;
	mIndicesNum = indices.size();

	UINT dataSize = sizeof(T) * mIndicesNum;

	mBuffer = CreateBufferResource(DEVICE, CMDLIST, (void*)indices.data(),
		dataSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &mUploadBuffer);

	mIndexBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	mIndexBufferView.SizeInBytes = dataSize;
}
