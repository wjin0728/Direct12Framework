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

	Vec3 CalculateNormal(int x, int z, CHeightMap* heightMapImage);

public:
	CHeightMapGridMesh() = default;
	CHeightMapGridMesh(int xStart, int zStart, int nWidth, int nLength, CHeightMap* heightMapImage);
	virtual ~CHeightMapGridMesh();
	int GetWidth() const { return width; }
	int GetLength() const { return height; }
};

