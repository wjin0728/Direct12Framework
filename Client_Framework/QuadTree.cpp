#include "stdafx.h"
#include "QuadTree.h"
#include"DX12Manager.h"
#include"GameObject.h"
#include"Camera.h"
#include"ResourceManager.h"


CQuadTree::~CQuadTree()
{
}

void CQuadTree::Initialize(const std::shared_ptr<CHeightMapGridMesh>& terrainMesh)
{
	mTerrainMesh = terrainMesh;
	heightMapWidth = terrainMesh->GetWidth();
	size_t vertexNum = mTerrainMesh->GetVertices().size();
	UINT heightMapHeight = vertexNum / heightMapWidth;

	std::array<UINT, 4> rootCorners{};
	rootCorners[CORNER_BL] = 0;
	rootCorners[CORNER_BR] = heightMapWidth - 1;
	rootCorners[CORNER_TL] = vertexNum - heightMapWidth;
	rootCorners[CORNER_TR] = vertexNum - 1;

	SubDivide(mRoot, rootCorners);
}

void CQuadTree::ReleaseUploadBuffer(std::shared_ptr<Node>& node)
{
	if (node->isLeaf) {
		node->mVertexBuffer->ReleaseUploadBuffer();
	}
	else {
		for (auto& child : node->mChildren) {
			ReleaseUploadBuffer(child);
		}
	}
}

void CQuadTree::ReleaseBuffer(std::shared_ptr<Node>& node)
{
	if (node->isLeaf) {
		node->mVertexBuffer = nullptr;
	}
	else {
		for (auto& child : node->mChildren) {
			ReleaseBuffer(child);
		}
	}
}

void CQuadTree::Render(const std::shared_ptr<class CCamera>& camera)
{
	if (camera) {
		RenderNode(mRoot, camera);
	}
}

void CQuadTree::RenderNode(const std::shared_ptr<Node>& node, const std::shared_ptr<class CCamera>& camera)
{
	/*if (!camera->IsInFrustum(node->aabb)) {
		return;
	}*/

	if (!node->isLeaf) {
		for (auto& child : node->mChildren) {
			RenderNode(child, camera);
		}
	}
	else {
		CMDLIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		node->mVertexBuffer->SetVertexBuffer();
		CMDLIST->DrawInstanced(4, 1, 0, 0);
	}
}

UINT CQuadTree::CalculateTriangleCount(const std::array<UINT, 4>& corners) const
{
	UINT width = corners[CORNER_TR] - corners[CORNER_TL];
	UINT height = (corners[CORNER_TL] - corners[CORNER_BL]) / heightMapWidth;

	UINT triangleCount = width * height * 2;

	return triangleCount;
}

BoundingBox CQuadTree::CalculateBoundingBox(const std::array<UINT, 4>& corners)
{
	auto& vertices = mTerrainMesh->GetVertices();

	UINT verWidthCnt = corners[CORNER_TR] - corners[CORNER_TL] + 1;
	UINT verHeightCnt = (corners[CORNER_TL] - corners[CORNER_BL]) / heightMapWidth + 1;

	float maxDepth = FLT_MIN;
	for (UINT i = 0; i < verHeightCnt; ++i) {
		for (UINT j = 0; j < verWidthCnt; ++j) {
			UINT idx = corners[CORNER_BL] + (i * heightMapWidth) + j;
			float depth = vertices[idx].position.y;

			if (depth > maxDepth) maxDepth = depth;
		}
	}
	Vec3 minPos = vertices[corners[CORNER_BL]].position;
	minPos.y = 0.f;
	Vec3 maxPos = vertices[corners[CORNER_TR]].position;
	maxPos.y = maxDepth;

	BoundingBox aabb{};
	BoundingBox::CreateFromPoints(aabb, minPos, maxPos);

	return aabb;
}

void CQuadTree::SubDivide(std::shared_ptr<Node>& node, const std::array<UINT, 4>& corners)
{
	UINT center = (corners[CORNER_TL] + corners[CORNER_TR] + corners[CORNER_BL] + corners[CORNER_BR]) / 4;
	node = std::make_shared<Node>(corners, center);

	UINT triangleCnt = CalculateTriangleCount(corners);
	if (triangleCnt == 0) {
		node->isLeaf = true;

		return;
	}
	else if (triangleCnt <= MAX_TRIANGLE_COUNT) {
		CreateGridMesh(node);
		node->aabb = CalculateBoundingBox(node->mCorners);
		node->isLeaf = true;

		return;
	}

	UINT topEdgeCenter = (corners[CORNER_TL] + corners[CORNER_TR]) / 2;
	UINT bottomEdgeCenter = (corners[CORNER_BL] + corners[CORNER_BR]) / 2;
	UINT leftEdgeCenter = (corners[CORNER_TL] + corners[CORNER_BL]) / 2;
	UINT rightEdgeCenter = (corners[CORNER_TR] + corners[CORNER_BR]) / 2;

	{
		std::array<UINT, 4> childCorner{ corners[CORNER_TL], topEdgeCenter, leftEdgeCenter, center };

		SubDivide(node->mChildren[CORNER_TL], childCorner);
	}
	{
		std::array<UINT, 4> childCorner{ topEdgeCenter, corners[CORNER_TR], center, rightEdgeCenter };
		SubDivide(node->mChildren[CORNER_TR], childCorner);
	}
	{
		std::array<UINT, 4> childCorner{ leftEdgeCenter, center, corners[CORNER_BL], bottomEdgeCenter };
		SubDivide(node->mChildren[CORNER_BL], childCorner);
	}
	{
		std::array<UINT, 4> childCorner{ center, rightEdgeCenter, bottomEdgeCenter, corners[CORNER_BR] };
		SubDivide(node->mChildren[CORNER_BR], childCorner);
	}

	BoundingBox aabb1{}, aabb2{};
	BoundingBox::CreateMerged(aabb1, node->mChildren[CORNER_TL]->aabb, node->mChildren[CORNER_TR]->aabb);
	BoundingBox::CreateMerged(aabb2, node->mChildren[CORNER_BL]->aabb, node->mChildren[CORNER_BR]->aabb);
	BoundingBox::CreateMerged(node->aabb, aabb1, aabb2);

	return;
}

void CQuadTree::CreateGridMesh(std::shared_ptr<Node>& node)
{
	auto& terrainVertices = mTerrainMesh->GetVertices();

	node->mVertices.resize(4);

	node->mVertices[CORNER_TL] = terrainVertices[node->mCorners[CORNER_TL]];
	node->mVertices[CORNER_TR] = terrainVertices[node->mCorners[CORNER_TR]];
	node->mVertices[CORNER_BL] = terrainVertices[node->mCorners[CORNER_BL]];
	node->mVertices[CORNER_BR] = terrainVertices[node->mCorners[CORNER_BR]];

	node->mVertexBuffer = std::make_shared<CVertexBuffer>();

	INSTANCE(CDX12Manager).OpenCommandList();
	node->mVertexBuffer->CreateBuffer(node->mVertices);
	INSTANCE(CDX12Manager).CloseCommandList();
}

