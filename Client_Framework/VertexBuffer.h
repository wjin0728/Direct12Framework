#pragma once
#include"DX12Manager.h"

class CVertexBuffer
{
private:
	friend class CMesh;
	friend class CSkinnedMesh;

	ComPtr<ID3D12Resource> mBuffer{};
	ComPtr<ID3D12Resource> mUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};

	UINT mSlot{};
	UINT mStride{};
	UINT mOffset{};
	UINT mVerticesNum{};

public:
	CVertexBuffer() = default;
	~CVertexBuffer() = default;

public:
	template<typename T>
	void CreateBuffer(const std::vector<T>& vertices, UINT slot = 0);
	void ReleaseUploadBuffer();

	void SetVertexBuffer() const;

public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return mVertexBufferView; }
};

template<typename T>
inline void CVertexBuffer::CreateBuffer(const std::vector<T>& vertices, UINT slot)
{
	mSlot = slot;
	mStride = sizeof(T);
	mVerticesNum = vertices.size();

	UINT dataSize = mStride * mVerticesNum;

	mBuffer = CreateBufferResource(DEVICE, CMDLIST, (void*)vertices.data(), dataSize,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &mUploadBuffer);

	mVertexBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
	mVertexBufferView.StrideInBytes = mStride;
	mVertexBufferView.SizeInBytes = dataSize;
}
