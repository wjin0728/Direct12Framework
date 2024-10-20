#pragma once
#include"stdafx.h"
#include "Mesh.h"

enum : UINT {
	TEXTURE_REPEAT_COUNT = 2
};

class CHeightMap;

class CHeightMapGridMesh : public CMesh
{
private:
	BYTE* heightMap;

	UINT width{};
	UINT height{};
	Vec3 scale{};

private:
	void LoadHeightMap(const std::wstring& fileName);
	void CalculateNormal();
	void CalculateTextureCoord();

public:
	CHeightMapGridMesh() = default;
	CHeightMapGridMesh(const std::wstring& fileName, int _width, int _height, Vec3 _scale);
	virtual ~CHeightMapGridMesh();

	int GetWidth() const { return width; }
	float GetHeight(float fx, float fz);
};

