#pragma once
#include"stdafx.h"

enum class CONSTANT_BUFFER_TYPE : UINT {
	PASS,
	OBJECT,
	LIGHT,
	MATERIAL,

	END
};

enum class STRUCTED_BUFFER_TYPE : UINT {
	END
};

enum class INSTANCE_BUFFER_TYPE : UINT {
	BILLBOARD,
	END
};

enum {
	UPLOAD_BUFFER_TYPE_COUNT = static_cast<UINT>(INSTANCE_BUFFER_TYPE::END),

	TEXTURE_COUNT = 100,
	CUBE_MAP_COUNT = 5,
	MATERIAL_COUNT = 10,
	MESH_COUNT = 50,
	PASS_COUNT = 2,
	OBJECT_COUNT = 500,
	BILLBOARD_COUNT = 2000
};


class CStructedBuffer
{
protected:
	ComPtr<ID3D12Resource> buffer{};

	BYTE* mappedData{};
	UINT rootParamIdx{};

	UINT bufferSize{};
	UINT dataNum{};
	UINT dataSize{};

public:
	CStructedBuffer() {};
	~CStructedBuffer();

public:
	void Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum = 1);
	void UpdateBuffer(UINT idx, const void* data);
	void BindToShader();

protected:
	void CreateBuffer();
};

class CConstantBuffer
{
protected:
	ComPtr<ID3D12Resource> buffer{};

	BYTE* mappedData{};
	UINT rootParamIdx{};

	UINT bufferSize{};
	UINT mNextOffset{};

public:
	CConstantBuffer() {};
	~CConstantBuffer();

public:
	void Initialize(UINT _rootParamIdx, UINT initialSize);
	UINT AddData(const void* data, UINT dataSize);
	void UpdateBuffer(UINT offSet, const void* data, UINT dataSize);
	void BindToShader(UINT offSet = 0);

protected:
	void CreateBuffer();
	void Resize(UINT size);
};
