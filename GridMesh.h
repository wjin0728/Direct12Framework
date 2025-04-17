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
	std::vector<float> heightData;

	UINT resolution{};
	Vec3 scale{};
	Vec3 offset{};

	std::shared_ptr<class CTexture> heightMapTex;

private:
	void LoadHeightMap(const std::string& fileName);
	void CreateHeightMapSRV();
	void CalculateNormal();
	void CalculateTextureCoord();

public:
	CHeightMapGridMesh() = default;
	void Initialize(const std::string& fileName, UINT _resoultion, Vec3 _scale, Vec3 offset = {0.f,0.f,0.f});
	virtual ~CHeightMapGridMesh();

	int GetWidth() const { return resolution; }
	float GetHeight(float fx, float fz);
	Vec3 GetScale() const { return scale; }
	Vec3 GetOffset() const { return offset; }
	UINT GetHeightMapIdx() const { return heightMapTex->GetSrvIndex(); }
};

