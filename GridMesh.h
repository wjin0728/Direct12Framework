#pragma once
#include"stdafx.h"
#include "Mesh.h"

enum : UINT {
	TEXTURE_REPEAT_COUNT = 1
};


class CHeightMapGridMesh : public CMesh
{
private:
	friend class CTerrain;
	std::vector<uint16_t> heightData;

	UINT resolution{};
	Vec3 scale{};

	D3D12_GPU_VIRTUAL_ADDRESS heightMapSRV;

private:
	void LoadHeightMap(const std::wstring& fileName);
	void CreateHeightMapSRV();
	void CalculateNormal();
	void CalculateTextureCoord();

public:
	CHeightMapGridMesh() = default;
	void Initialize(const std::wstring& fileName, UINT _resoultion, Vec3 _scale);
	virtual ~CHeightMapGridMesh();

	int GetWidth() const { return resolution; }
	float GetHeight(float fx, float fz);
	Vec3 GetScale() const { return scale; }
};

