#pragma once
#include"stdafx.h"

enum class CONSTANT_BUFFER_TYPE : UINT {
	PASS,
	OBJECT,
	LIGHT,
	BONE_TRANSFORM,
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

	TEXTURE_COUNT = 200,
	CUBE_MAP_COUNT = 5,
	MATERIAL_COUNT = 10,
	MESH_COUNT = 50,
	PASS_COUNT = 2,
	OBJECT_COUNT = 4000,
	BILLBOARD_COUNT = 2000,
	BONE_TRANSFORM_COUNT = 10
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
	template<typename T>
	UINT AddData(const T* data);
	void UpdateBuffer(UINT offSet, const void* data, UINT dataSize);
	template<typename T>
	void UpdateBuffer(UINT offSet, const T* data);
	void BindToShader(UINT offSet = 0);

protected:
	void CreateBuffer();
	void Resize(UINT size);
};

template<typename T>
inline UINT CConstantBuffer::AddData(const T* data)
{
	size_t dataSize = sizeof(T);

	UINT currentOffset = mNextOffset;
	mNextOffset += ALIGNED_SIZE(dataSize);
	if (mNextOffset >= bufferSize) {
		Resize(mNextOffset * 1.5f);
	}
	memcpy(&mappedData[currentOffset], data, dataSize);

	return currentOffset;
}

template<typename T>
inline void CConstantBuffer::UpdateBuffer(UINT offSet, const T* data)
{
	size_t dataSize = sizeof(T);
	memcpy(&mappedData[offSet], data, dataSize);
}
