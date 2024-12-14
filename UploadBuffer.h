#pragma once
#include"stdafx.h"

enum class CONSTANT_BUFFER_TYPE : UINT {
	PASS,
	OBJECT,
	LIGHT,

	END
};

enum class STRUCTED_BUFFER_TYPE : UINT {
	MATERIAL = static_cast<UINT>(CONSTANT_BUFFER_TYPE::END),
	END
};

enum class INSTANCE_BUFFER_TYPE : UINT {
	BILLBOARD = static_cast<UINT>(STRUCTED_BUFFER_TYPE::END),
	END
};

enum {
	UPLOAD_BUFFER_TYPE_COUNT = static_cast<UINT>(INSTANCE_BUFFER_TYPE::END),

	TEXTURE_COUNT = 100,
	CUBE_MAP_COUNT = 5,
	MATERIAL_COUNT = 1000,
	MESH_COUNT = 50,
	PASS_COUNT = 1,
	OBJECT_COUNT = 500,
	BILLBOARD_COUNT = 20000
};


class CUploadBuffer
{
protected:
	ComPtr<ID3D12Resource> buffer{};

	BYTE* mappedData{};
	UINT rootParamIdx{};

	UINT dataNum{};
	UINT byteSize{};
	UINT dataSize{};

public:
	CUploadBuffer() {};
	virtual ~CUploadBuffer();

public:
	virtual void Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum = 1) = 0;

	void CopyData(const void* _data, UINT idx = 0);
	virtual void UpdateBuffer(UINT idx = 0) {};

protected:
	void CreateBuffer();
};


class CConstantBuffer : public CUploadBuffer
{
public:
	CConstantBuffer() {};
	virtual ~CConstantBuffer() {};

public:
	virtual void Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum = 1);
	virtual void UpdateBuffer(UINT idx = 0);
};

class CStructedBuffer : public CUploadBuffer
{
public:
	CStructedBuffer() {};
	virtual ~CStructedBuffer() {};

public:
	virtual void Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum = 1);
	virtual void UpdateBuffer(UINT idx = 0);
};