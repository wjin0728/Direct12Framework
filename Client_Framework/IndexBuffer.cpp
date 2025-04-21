#include "stdafx.h"
#include "IndexBuffer.h"
#include"DX12Manager.h"

void CIndexBuffer::ReleaseUploadBuffer()
{
	if (mUploadBuffer)
		mUploadBuffer.Reset();
}

void CIndexBuffer::ReleaseBuffer()
{
	if (mBuffer)
		mBuffer.Reset();
}

void CIndexBuffer::SetIndexBuffer() const
{
	CMDLIST->IASetIndexBuffer(&mIndexBufferView);
}
