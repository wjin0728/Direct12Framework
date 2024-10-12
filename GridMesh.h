#pragma once
#include"stdafx.h"
#include "Mesh.h"

class CHeightMapImage;

class CHeightMapGridMesh : public CMesh
{
private:
	int width;
	int height;

private:

	XMFLOAT3 CalculateNormal(int x, int z, CHeightMapImage* heightMapImage);

public:
	BoundingBox aabb;

	CHeightMapGridMesh() = default;
	CHeightMapGridMesh(int xStart, int zStart, int nWidth, int nLength, CHeightMapImage* heightMapImage);
	virtual ~CHeightMapGridMesh();
	int GetWidth() { return width; }
	int GetLength() { return height; }
};

