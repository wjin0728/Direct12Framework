#pragma once
#include"stdafx.h"
#include "Mesh.h"

class CHeightMap;

class CHeightMapGridMesh : public CMesh
{
private:
	UINT width;
	UINT height;

private:

	XMFLOAT3 CalculateNormal(int x, int z, CHeightMap* heightMapImage);

public:
	BoundingBox aabb;

	CHeightMapGridMesh() = default;
	CHeightMapGridMesh(int xStart, int zStart, int nWidth, int nLength, CHeightMap* heightMapImage);
	virtual ~CHeightMapGridMesh();
	int GetWidth() { return width; }
	int GetLength() { return height; }
};

