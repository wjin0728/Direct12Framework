#pragma once
#include"stdafx.h"
#include"GridMesh.h"
#include"HeightMapImage.h"

enum : UINT
{
	CORNER_TL,
	CORNER_TR,
	CORNER_BL,
	CORNER_BR,


	MAX_TRIANGLE_COUNT = 8192
};

class CQuadTree
{
private:
	struct Node
	{
		friend CQuadTree;

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		D3D12_INDEX_BUFFER_VIEW indexBufferView{};

		UINT mCenter{};
		std::array<UINT, 4> mCorners{};
		std::array<std::shared_ptr<Node>, 4> mChildren{};

		Vec2 position{}; //x, z
		float width{};
		bool isLeaf = false;

		Node(const std::array<UINT, 4>& corners, UINT center) : mCorners(corners), mCenter(center) {}
	};

private:
	std::vector<CVertex> terrainVertices{};
	std::unique_ptr<Node> root{};
	
	UINT heightMapWidth{};

public:
	CQuadTree() {};
	~CQuadTree() {};

	void Initialize(const std::shared_ptr<CHeightMapGridMesh>& terrainMesh);

private:
	void SubDivide(std::shared_ptr<Node>& node, const std::array<UINT, 4>& corners);
	void CreateGridMesh(std::shared_ptr<Node>& node);
};

