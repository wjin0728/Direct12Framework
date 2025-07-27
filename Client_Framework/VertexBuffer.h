#pragma once
#include"UploadBuffer.h"

class CVertexBuffer : public CStructedBuffer
{
private:
	friend class CMesh;
	friend class CSkinnedMesh;

	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};

	UINT mSlot{};
	UINT mStride{};
	UINT mOffset{};

	bool mIsDynamic = false;

public:
	bool mIsActive = false;
	CVertexBuffer() : CStructedBuffer(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER) {}
	virtual ~CVertexBuffer() = default;

public:
	template<typename T>
	void CreateBuffer(const std::vector<T>& vertices, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, UINT slot = 0, bool isDynamic = false);

	void UpdateVertexBuffer(const void* data, UINT size);
	void SetVertexBuffer() const;

	void ClearBuffer()
	{
		mVertexBufferView = {};
		mIsActive = false;
	}

public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return mVertexBufferView; }
};

template<typename T>
inline void CVertexBuffer::CreateBuffer(const std::vector<T>& vertices, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, UINT slot, bool isDynamic)
{
	if (vertices.empty()) return;
	mIsDynamic = isDynamic;
	mSlot = slot;
	dataSize = sizeof(T);
	dataNum = vertices.size();

	bufferSize = dataSize * dataNum;

	buffer = CreateBufferResource(device, cmdList, (void*)vertices.data(), bufferSize,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &uploadBuffer);

	mVertexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	mVertexBufferView.StrideInBytes = dataSize;
	mVertexBufferView.SizeInBytes = bufferSize;
}
