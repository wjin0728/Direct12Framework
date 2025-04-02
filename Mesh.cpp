#include "stdafx.h"
#include "Mesh.h"
#include<numeric>
#include"DX12Manager.h"


CVertex::CVertex(const Vec3& pos, const Vec3& nor) : normal(nor)
{
	position = pos;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//


CMesh::CMesh(std::ifstream& inFile)
{
	int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0;
	int nvertices = 0;

	BinaryReader::ReadDateFromFile(inFile, nvertices);

	vertices.resize(nvertices);

	std::string meshName;
	BinaryReader::ReadDateFromFile(inFile, meshName);

	std::string token;

	for (; ; )
	{
		BinaryReader::ReadDateFromFile(inFile, token);

		if (token == "<Bounds>:")
		{
			Vec3 boundingCenter, boundingExtent;

			BinaryReader::ReadDateFromFile(inFile, boundingCenter);
			BinaryReader::ReadDateFromFile(inFile, boundingExtent);

			BoundingOrientedBox::CreateFromBoundingBox(oobb, BoundingBox(boundingCenter, boundingExtent));
			BoundingSphere::CreateFromBoundingBox(oobs, oobb);
		}
		else if (token == "<Positions>:")
		{
			BinaryReader::ReadDateFromFile(inFile, nPositions);

			for (int i = 0; i < nPositions; i++) {
				BinaryReader::ReadDateFromFile(inFile, vertices[i].position);
			}
		}
		else if (token == "<Colors>:")
		{
		}
		else if (token == "<Normals>:")
		{
			BinaryReader::ReadDateFromFile(inFile, nNormals);

			for (int i = 0; i < nNormals; i++) {
				BinaryReader::ReadDateFromFile(inFile, vertices[i].normal);
			}
		}
		else if (token == "<Indices>:")
		{
			BinaryReader::ReadDateFromFile(inFile, nIndices);

			indices.emplace_back(nIndices);
			inFile.read((char*)&indices[0][0], sizeof(UINT) * nIndices);
		}
		else if (token == "<SubMeshes>:")
		{
			BinaryReader::ReadDateFromFile(inFile, subMeshNum);

			if (subMeshNum > 0)
			{
				indices.clear();
				indices.resize(subMeshNum);

				for (int i = 0; i < subMeshNum; i++)
				{
					BinaryReader::ReadDateFromFile(inFile, token);
					if (token == "<SubMesh>:")
					{
						int nIndex = 0;
						int idxCnt = 0;
						BinaryReader::ReadDateFromFile(inFile, nIndex);
						BinaryReader::ReadDateFromFile(inFile, idxCnt);

						if (idxCnt > 0)
						{
							indices[i].resize(idxCnt);
							inFile.read((char*)&(indices[i][0]), sizeof(UINT) * idxCnt);
						}
					}
				}
			}
		}
		else if (token == "</Mesh>")
		{
			break;
		}
	}
}

void CMesh::CreateVertexBuffer()
{
	mVertexBuffer = std::make_shared<CVertexBuffer>();
	mVertexBuffer->CreateBuffer(vertices);
}

void CMesh::CreateIndexBuffers()
{
	for (const auto& idx : indices) {
		auto indexBuffer = std::make_shared<CIndexBuffer>();
		indexBuffer->CreateBuffer(idx);

		mIndexBuffers.push_back(indexBuffer);
	}
}

std::shared_ptr<CMesh> CMesh::CreateMeshFromFile(std::ifstream& inFile)
{
	std::shared_ptr<CMesh> m = std::make_shared<CMesh>();

	std::string name{};
	BinaryReader::ReadDateFromFile(inFile, name);

	std::string token;

	while (true)
	{
		BinaryReader::ReadDateFromFile(inFile, token);

		if (token == "<Bounds>:")
		{
			Vec3 boundingCenter, boundingExtent;

			BinaryReader::ReadDateFromFile(inFile, boundingCenter);
			BinaryReader::ReadDateFromFile(inFile, boundingExtent);

			BoundingOrientedBox::CreateFromBoundingBox(m->oobb, BoundingBox(boundingCenter, boundingExtent));
			BoundingSphere::CreateFromBoundingBox(m->oobs, m->oobb);
		}
		else if (token == "<Positions>:" || token == "<ControlPoints>:")
		{
			int nPositions{};
			BinaryReader::ReadDateFromFile(inFile, nPositions);
			m->vertices.resize(nPositions);

			for (int i = 0; i < nPositions; i++) {
				BinaryReader::ReadDateFromFile(inFile, m->vertices[i].position);
		}
		}
		else if (token == "<Colors>:")
		{

		}
		else if (token == "<Normals>:")
		{
			int nNormals{};
			BinaryReader::ReadDateFromFile(inFile, nNormals);

			for (int i = 0; i < nNormals; i++) {
				BinaryReader::ReadDateFromFile(inFile, m->vertices[i].normal);
	}
		}
		else if (token == "<TextureCoords0>:")
		{
			int nTextureCoord{};
			BinaryReader::ReadDateFromFile(inFile, nTextureCoord);

			for (int i = 0; i < nTextureCoord; i++) {
				BinaryReader::ReadDateFromFile(inFile, m->vertices[i].texCoord);
			}
		}
		else if (token == "<TextureCoords1>:")
		{
			int nTextureCoord{};
			BinaryReader::ReadDateFromFile(inFile, nTextureCoord);

			Vec2 temp;
			for (int i = 0; i < nTextureCoord; i++) {
				BinaryReader::ReadDateFromFile(inFile, temp);
			}
		}
		else if (token == "<Tangents>:")
		{
			int nTangent{};
			BinaryReader::ReadDateFromFile(inFile, nTangent);

			for (int i = 0; i < nTangent; i++) {
				BinaryReader::ReadDateFromFile(inFile, m->vertices[i].tangent);
			}

		}
		else if (token == "<BiTangents>:")
		{
			int nBiTangent{};
			Vec3 biTangent{};
			BinaryReader::ReadDateFromFile(inFile, nBiTangent);
			for (int i = 0; i < nBiTangent; i++) {
				BinaryReader::ReadDateFromFile(inFile, biTangent);
			}
		}
		else if (token == "<Indices>:")
		{
			int nIndices{};
			BinaryReader::ReadDateFromFile(inFile, nIndices);

			m->indices.emplace_back(nIndices);
			inFile.read((char*)&m->indices[0][0], sizeof(UINT) * nIndices);
		}
		else if (token == "<SubMeshes>:")
		{
			BinaryReader::ReadDateFromFile(inFile, m->subMeshNum);

			if (m->subMeshNum > 0)
			{
				m->indices.clear();
				m->indices.resize(m->subMeshNum);

				for (int i = 0; i < m->subMeshNum; i++)
				{
					BinaryReader::ReadDateFromFile(inFile, token);
					if (token == "<SubMesh>:")
					{
						int subMeshIdx = 0;
						BinaryReader::ReadDateFromFile(inFile, subMeshIdx);
						int nIndex = 0;
						BinaryReader::ReadDateFromFile(inFile, nIndex);

						if (nIndex > 0)
						{
							m->indices[i].resize(nIndex);
							inFile.read((char*)&(m->indices[i][0]), sizeof(UINT) * nIndex);
						}
					}
				}
			}
		}
		else if (token == "</Mesh>")
		{
			break;
		}
	}
	m->CreateVertexBuffer();
	m->CreateIndexBuffers();

	return m;
}

std::shared_ptr<CMesh> CMesh::CreateCubeMesh(Vec3 scale)
{
	std::shared_ptr<CMesh> m = std::make_shared<CMesh>();

	m->vertices.resize(24);
	m->primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = scale.x * 0.5f, fy = scale.y * 0.5f, fz = scale.z * 0.5f;

	//¾Õ¸é
	m->vertices[0] = CVertex(Vec3(-fx, -fy, -fz), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	m->vertices[1] = CVertex(Vec3(-fx, +fy, -fz), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	m->vertices[2] = CVertex(Vec3(+fx, +fy, -fz), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	m->vertices[3] = CVertex(Vec3(+fx, -fy, -fz), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	// µÞ¸é
	m->vertices[4] = CVertex(Vec3(-fx, -fy, +fz), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	m->vertices[5] = CVertex(Vec3(+fx, -fy, +fz), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	m->vertices[6] = CVertex(Vec3(+fx, +fy, +fz), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	m->vertices[7] = CVertex(Vec3(-fx, +fy, +fz), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// À­¸é
	m->vertices[8] = CVertex(Vec3(-fx, +fy, -fz), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	m->vertices[9] = CVertex(Vec3(-fx, +fy, +fz), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	m->vertices[10] = CVertex(Vec3(+fx, +fy, +fz), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	m->vertices[11] = CVertex(Vec3(+fx, +fy, -fz), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	// ¾Æ·§¸é			
	m->vertices[12] = CVertex(Vec3(-fx, -fy, -fz), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	m->vertices[13] = CVertex(Vec3(+fx, -fy, -fz), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	m->vertices[14] = CVertex(Vec3(+fx, -fy, +fz), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	m->vertices[15] = CVertex(Vec3(-fx, -fy, +fz), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// ¿ÞÂÊ¸é							
	m->vertices[16] = CVertex(Vec3(-fx, -fy, +fz), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	m->vertices[17] = CVertex(Vec3(-fx, +fy, +fz), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	m->vertices[18] = CVertex(Vec3(-fx, +fy, -fz), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	m->vertices[19] = CVertex(Vec3(-fx, -fy, -fz), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	// ¿À¸¥ÂÊ¸é		   					
	m->vertices[20] = CVertex(Vec3(+fx, -fy, -fz), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	m->vertices[21] = CVertex(Vec3(+fx, +fy, -fz), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	m->vertices[22] = CVertex(Vec3(+fx, +fy, +fz), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	m->vertices[23] = CVertex(Vec3(+fx, -fy, +fz), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

	m->indices.emplace_back(36);

	// ¾Õ¸é
	m->indices[0][0] = 0; m->indices[0][1] = 1; m->indices[0][2] = 2;
	m->indices[0][3] = 0; m->indices[0][4] = 2; m->indices[0][5] = 3;
	// µÞ¸é
	m->indices[0][6] = 4; m->indices[0][7] = 5; m->indices[0][8] = 6;
	m->indices[0][9] = 4; m->indices[0][10] = 6; m->indices[0][11] = 7;
	// À­¸é
	m->indices[0][12] = 8; m->indices[0][13] = 9; m->indices[0][14] = 10;
	m->indices[0][15] = 8; m->indices[0][16] = 10; m->indices[0][17] = 11;
	// ¾Æ·§¸é
	m->indices[0][18] = 12; m->indices[0][19] = 13; m->indices[0][20] = 14;
	m->indices[0][21] = 12; m->indices[0][22] = 14; m->indices[0][23] = 15;
	// ¿ÞÂÊ¸é
	m->indices[0][24] = 16; m->indices[0][25] = 17; m->indices[0][26] = 18;
	m->indices[0][27] = 16; m->indices[0][28] = 18; m->indices[0][29] = 19;
	// ¿À¸¥ÂÊ¸é
	m->indices[0][30] = 20; m->indices[0][31] = 21; m->indices[0][32] = 22;
	m->indices[0][33] = 20; m->indices[0][34] = 22; m->indices[0][35] = 23;

	m->CreateOOBBAndOOBS();

	return m;
}

std::shared_ptr<CMesh> CMesh::CreateRectangleMesh(Vec2 scale)
{
	std::shared_ptr<CMesh> m = std::make_shared<CMesh>();

	float fx = 0.5f * scale.x;
	float fy = 0.5f * scale.y;

	m->vertices.resize(4);

	// ¾Õ¸é
	m->vertices[0] = CVertex(Vec3(-fx, -fy, 0), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	m->vertices[1] = CVertex(Vec3(-fx, +fy, 0), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	m->vertices[2] = CVertex(Vec3(+fx, +fy, 0), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	m->vertices[3] = CVertex(Vec3(+fx, -fy, 0), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	m->indices.emplace_back(6);

	// ¾Õ¸é
	m->indices[0][0] = 0; m->indices[0][1] = 1; m->indices[0][2] = 2;
	m->indices[0][3] = 0; m->indices[0][4] = 2; m->indices[0][5] = 3;

	m->primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return m;
}

std::shared_ptr<CMesh> CMesh::CreateSphereMesh(float radius, UINT stackCnt, UINT sliceCnt)
{
	std::shared_ptr<CMesh> m = std::make_shared<CMesh>();

	CVertex v;

	v.position = Vec3(0.0f, radius, 0.0f);
	v.texCoord = Vec2(0.5f, 0.0f);
	v.normal = v.position.GetNormalized();
	v.tangent = Vec3(1.0f, 0.0f, 1.0f);

	m->vertices.push_back(v);

	float stackAngle = XM_PI / stackCnt;
	float sliceAngle = XM_2PI / sliceCnt;

	float deltaU = 1.f / static_cast<float>(sliceCnt);
	float deltaV = 1.f / static_cast<float>(stackCnt);

	for (UINT y = 1; y <= stackCnt - 1; ++y)
	{
		float phi = y * stackAngle;

		for (UINT x = 0; x <= sliceCnt; ++x)
		{
			float theta = x * sliceAngle;

			v.position.x = radius * sinf(phi) * cosf(theta);
			v.position.y = radius * cosf(phi);
			v.position.z = radius * sinf(phi) * sinf(theta);

			v.texCoord = Vec2(deltaU * x, deltaV * y);

			v.normal = v.position.GetNormalized();

			v.tangent.x = -radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sinf(phi) * cosf(theta);
			v.tangent = v.tangent.GetNormalized();

			m->vertices.push_back(v);
		}
	}

	v.position = Vec3(0.0f, -radius, 0.0f);
	v.texCoord = Vec2(0.5f, 1.0f);
	v.normal = v.position.GetNormalized();
	v.tangent = Vec3(1.0f, 0.0f, 0.0f);

	m->vertices.push_back(v);

	m->indices.emplace_back(36);

	for (UINT i = 0; i <= sliceCnt; ++i)
	{
		m->indices[0].push_back(0);
		m->indices[0].push_back(i + 2);
		m->indices[0].push_back(i + 1);
	}

	UINT ringVertexCount = sliceCnt + 1;
	for (UINT y = 0; y < stackCnt - 2; ++y)
	{
		for (UINT x = 0; x < sliceCnt; ++x)
		{
			m->indices[0].push_back(1 + (y)*ringVertexCount + (x));
			m->indices[0].push_back(1 + (y)*ringVertexCount + (x + 1));
			m->indices[0].push_back(1 + (y + 1) * ringVertexCount + (x));
			m->indices[0].push_back(1 + (y + 1) * ringVertexCount + (x));
			m->indices[0].push_back(1 + (y)*ringVertexCount + (x + 1));
			m->indices[0].push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	UINT bottomIndex = static_cast<UINT>(m->vertices.size()) - 1;
	UINT lastRingStartIndex = bottomIndex - ringVertexCount;
	for (UINT i = 0; i < sliceCnt; ++i)
	{
		m->indices[0].push_back(bottomIndex);
		m->indices[0].push_back(lastRingStartIndex + i);
		m->indices[0].push_back(lastRingStartIndex + i + 1);
	}

	m->CreateOOBBAndOOBS();

	return m;
}

void CMesh::CreateOOBBAndOOBS()
{
	BoundingOrientedBox::CreateFromPoints(oobb, vertices.size(), (Vec3*)vertices.data(), sizeof(CVertex));
	BoundingSphere::CreateFromPoints(oobs, vertices.size(), (Vec3*)vertices.data(), sizeof(CVertex));
}

void CMesh::CalculateNormal()
{
	for (auto& v : vertices) {
		v.normal = Vec3(0.f, 0.f, 0.f);
	}
	int nOffset = (primitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	UINT nVertices = vertices.size();

	int nPrimitives = (primitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ?
		(nVertices / 3) : (nVertices - 2);

	UINT nIndices = indices[curSubSet].size();

	if (nIndices) nPrimitives = (primitiveTopology ==
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (nIndices / 3) : (nIndices - 2);

	for (int i = 0; i < nPrimitives; i++)
	{
		UINT v0Idx = ((nIndices) ? (indices[curSubSet][(i * nOffset) + 0]) : ((i * nOffset) + 0));
		UINT v1Idx = ((nIndices) ? (indices[curSubSet][(i * nOffset) + 1]) : ((i * nOffset) + 1));
		UINT v2Idx = ((nIndices) ? (indices[curSubSet][(i * nOffset) + 2]) : ((i * nOffset) + 2));

		Vec3 v0 = vertices[v0Idx].position;
		Vec3 v1 = vertices[v1Idx].position;
		Vec3 v2 = vertices[v2Idx].position;

		Vec3 edge1 = v1 - v0;
		Vec3 edge2 = v2 - v0;

		Vec3 normal = edge1.Cross(edge2).GetNormalized();

		vertices[v0Idx].normal = (vertices[v0Idx].normal + normal).GetNormalized();
		vertices[v1Idx].normal = (vertices[v1Idx].normal + normal).GetNormalized();
		vertices[v2Idx].normal = (vertices[v2Idx].normal + normal).GetNormalized();
	}
}

void CMesh::ReleaseUploadBuffer()
{
	mVertexBuffer->ReleaseUploadBuffer();
	for (auto& indexBuffer : mIndexBuffers) {
		indexBuffer->ReleaseUploadBuffer();
	}
}


int CMesh::CheckRayIntersection(const Ray& ray, float& nearHitDistance)
{
	int nIntersections = 0;
	int nOffset = (primitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;

	UINT nVertices = vertices.size();

	int nPrimitives = (primitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ?
		(nVertices / 3) : (nVertices - 2);

	UINT nIndices = indices[curSubSet].size();

	if (nIndices) nPrimitives = (primitiveTopology ==
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (nIndices / 3) : (nIndices - 2);

	bool intersected = ray.Intersects(oobb, nearHitDistance);

	if (intersected)
	{
		float fNearHitDistance = FLT_MAX;

		for (int i = 0; i < nPrimitives; i++)
		{
			UINT v0Idx = ((nIndices) ? (indices[curSubSet][(i * nOffset) + 0]) : ((i * nOffset) + 0));
			UINT v1Idx = ((nIndices) ? (indices[curSubSet][(i * nOffset) + 1]) : ((i * nOffset) + 1));
			UINT v2Idx = ((nIndices) ? (indices[curSubSet][(i * nOffset) + 2]) : ((i * nOffset) + 2));

			float fHitDistance;
			bool bIntersected = ray.Intersects(vertices[v0Idx].position, vertices[v1Idx].position, vertices[v2Idx].position, fHitDistance);

			if (bIntersected)
			{
				if (fHitDistance < fNearHitDistance)
				{
					nearHitDistance = fNearHitDistance = fHitDistance;
				}
				nIntersections++;
			}
		}
	}
	return(nIntersections);
}

void CMesh::Render(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->IASetPrimitiveTopology(primitiveTopology);
	mVertexBuffer->SetVertexBuffer();

	if (!indices.empty() && indices.size() > curSubSet)
	{
		mIndexBuffers[curSubSet]->SetIndexBuffer();
		cmdList->DrawIndexedInstanced(indices[curSubSet].size(), 1, 0, 0, 0);
	}
	else
	{
		cmdList->DrawInstanced(vertices.size(), 1, mVertexBuffer->mOffset, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* cmdList, int idx)
{
	cmdList->IASetPrimitiveTopology(primitiveTopology);
	mVertexBuffer->SetVertexBuffer();

	if (!indices.empty() && indices.size() > idx)
	{
		mIndexBuffers[idx]->SetIndexBuffer();
		cmdList->DrawIndexedInstanced(indices[idx].size(), 1, 0, 0, 0);
	}
	else
	{
		cmdList->DrawInstanced(vertices.size(), 1, mVertexBuffer->mOffset, 0);
	}
}

void CMesh::Render(D3D12_VERTEX_BUFFER_VIEW ibv, UINT instancingNum, int idx)
{
	CMDLIST->IASetPrimitiveTopology(primitiveTopology);
	D3D12_VERTEX_BUFFER_VIEW instanceData[] = { mVertexBuffer->GetVertexBufferView(), ibv };
	CMDLIST->IASetVertexBuffers(0, 2, instanceData);

	if (!indices.empty() && indices.size() > idx)
	{
		mIndexBuffers[idx]->SetIndexBuffer();
		CMDLIST->DrawIndexedInstanced(indices[idx].size(), instancingNum, 0, 0, 0);
	}
	else
	{
		CMDLIST->DrawInstanced(vertices.size(), instancingNum, mVertexBuffer->mOffset, 0);
	}
}
