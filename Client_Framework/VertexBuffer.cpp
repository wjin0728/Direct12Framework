#include "stdafx.h"
#include "VertexBuffer.h"
#include"DX12Manager.h"


void CVertexBuffer::UpdateVertexBuffer(const void* data, UINT size)
{
	dataNum = size;
	bufferSize = dataSize * dataNum;

	if (!mappedData) return;
	if (data) memcpy(&mappedData, data, dataNum);

	mVertexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	mVertexBufferView.StrideInBytes = dataSize;
	mVertexBufferView.SizeInBytes = bufferSize;
}

void CVertexBuffer::SetVertexBuffer() const
{
	CMDLIST->IASetVertexBuffers(mSlot, 1, &mVertexBufferView);
}
