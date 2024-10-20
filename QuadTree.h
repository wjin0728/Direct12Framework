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
		
		D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};

		ComPtr<ID3D12Resource> mIndexBuffer{};
		ComPtr<ID3D12Resource> mIndexUploadBuffer{};
		D3D12_INDEX_BUFFER_VIEW mIndexBufferView{};
		std::vector<UINT> mIndices;
		size_t indexCnt{};

		UINT mCenter{};
		std::array<UINT, 4> mCorners{};
		std::array<std::shared_ptr<Node>, 4> mChildren{};

		BoundingBox aabb{};
		bool isLeaf = false;

		Node(const std::array<UINT, 4>& corners, UINT center) : mCorners(corners), mCenter(center) {}
	};

private:
	std::shared_ptr<CHeightMapGridMesh> mTerrainMesh{};
	std::shared_ptr<Node> mRoot{};
	
	UINT heightMapWidth{};

public:
	CQuadTree() {};
	~CQuadTree() {};

	void Initialize(const std::shared_ptr<CHeightMapGridMesh>& terrainMesh);

	void ReleaseUploadBuffer(std::shared_ptr<Node>& node);

	void Render(const std::shared_ptr<class CGameObject> cameraObject);
	void RenderNode(const std::shared_ptr<Node>& node, const std::shared_ptr<class CCamera> camera);

private:
	UINT CalculateTriangleCount(const std::array<UINT, 4>& corners) const;
	BoundingBox CalculateBoundingBox(const std::array<UINT, 4>& corners);

	void SubDivide(std::shared_ptr<Node>& node, const std::array<UINT, 4>& corners);
	void CreateGridMesh(std::shared_ptr<Node>& node);
};

