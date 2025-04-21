#include "stdafx.h"
#include "VertexBuffer.h"
#include"DX12Manager.h"

void CVertexBuffer::ReleaseUploadBuffer()
{
	if (mUploadBuffer)
		mUploadBuffer.Reset();
}

void CVertexBuffer::SetVertexBuffer() const
{
	CMDLIST->IASetVertexBuffers(mSlot, 1, &mVertexBufferView);
}
