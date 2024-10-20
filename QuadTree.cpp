#include "stdafx.h"
#include "QuadTree.h"


void CQuadTree::Initialize(const std::shared_ptr<CHeightMapGridMesh>& terrainMesh)
{
	terrainVertices = terrainMesh->GetVertices();

	
}

void CQuadTree::SubDivide(std::shared_ptr<Node>& node, const std::array<UINT, 4>& corners)
{
	UINT center = (corners[CORNER_TL] + corners[CORNER_TR] + corners[CORNER_BL] + corners[CORNER_BR]) / 4;
	node = std::make_shared<Node>(corners, center);

	UINT n = corners[CORNER_TR] - corners[CORNER_TL];
	if (pow(2, 2 * n - 1) <= MAX_TRIANGLE_COUNT)
	{
		CreateGridMesh(node);
		node->isLeaf = true;

		return;
	}

	UINT topEdgeCenter = (corners[CORNER_TL] + corners[CORNER_TR]) / 2;
	UINT bottomEdgeCenter = (corners[CORNER_BL] + corners[CORNER_BR]) / 2;
	UINT leftEdgeCenter = (corners[CORNER_TL] + corners[CORNER_BL]) / 2;
	UINT rightEdgeCenter = (corners[CORNER_TR] + corners[CORNER_BR]) / 2;

	{
		std::array<UINT, 4> corners{ corners[CORNER_TL], topEdgeCenter, leftEdgeCenter, center };
		SubDivide(node->mChildren[CORNER_TL], corners);
	}
	{
		std::array<UINT, 4> corners{ topEdgeCenter, corners[CORNER_TR], center, rightEdgeCenter };
		SubDivide(node->mChildren[CORNER_TR], corners);
	}
	{
		std::array<UINT, 4> corners{ leftEdgeCenter, center, corners[CORNER_BL], bottomEdgeCenter };
		SubDivide(node->mChildren[CORNER_BL], corners);
	}
	{
		std::array<UINT, 4> corners{ center, rightEdgeCenter, bottomEdgeCenter, corners[CORNER_BR] };
		SubDivide(node->mChildren[CORNER_BR], corners);
	}

	return;
}

void CQuadTree::CreateGridMesh(std::shared_ptr<Node>& node)
{
	UINT width = node->mCorners[CORNER_TR] - node->mCorners[CORNER_TL] + 1;
	UINT height = (node->mCorners[CORNER_TL] / heightMapWidth) - (node->mCorners[CORNER_BL] / heightMapWidth) + 1;

	//정점 설정
	size_t verticesNum = static_cast<size_t>(width) * height;
	std::vector<CVertex> vertices(verticesNum);

	UINT idx = 0;
	for (UINT i = 0; i < height; ++i) {
		for (UINT j = 0; j < width; ++j) {
			UINT terIdx = node->mCorners[CORNER_BL] + (width * i) + j;
			vertices[idx++] = terrainVertices[terIdx];
		}
	}

	//인덱스 설정
	size_t indicesNum = (static_cast<size_t>(width) - 1) * (static_cast<size_t>(height) - 1) * 6;
	std::vector<UINT> indices(indicesNum);


}

