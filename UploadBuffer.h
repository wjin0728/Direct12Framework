#pragma once
#include"stdafx.h"

enum class CONSTANT_BUFFER_TYPE : UINT {
	PASS,
	OBJECT,
	LIGHT,
	MATERIAL,

	END
};

enum {
	CONSTANT_BUFFER_TYPE_COUNT = CONSTANT_BUFFER_TYPE::END,

	MATERIAL_COUNT = 100,
	PASS_COUNT = 1,
	OBJECT_COUNT = 100
};

class CUploadBuffer
{
private:
	ComPtr<ID3D12Resource> buffer = ComPtr<ID3D12Resource>();

	BYTE* mappedData{};
	UINT rootParamIdx{};

	UINT dataNum{};
	UINT byteSize{};
	UINT dataSize{};

public:
	CUploadBuffer() {};
	~CUploadBuffer();

public:
	void Initialize(UINT _rootParamIdx, UINT _dataSize, UINT _dataNum = 1);

	void UpdateData(const void* _data, UINT idx = 0, UINT _dataNum = 1);

	void UpdateConstantBuffer(UINT idx = 0);

private:
	void CreateBuffer();
};

