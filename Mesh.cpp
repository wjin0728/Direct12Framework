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

void CMesh::CreateBufferViews()
{
	auto device = INSTANCE(CDX12Manager).GetDevice();
	auto cmdList = INSTANCE(CDX12Manager).GetCommandList();

	CreateVertexBufferView(device, cmdList);
	CreateIndexBufferView(device, cmdList);
}

std::shared_ptr<CMesh> CMesh::CreateMeshFromFile(std::ifstream& inFile)
{
	std::shared_ptr<CMesh> m = std::make_shared<CMesh>();

	int nvertices{};
	BinaryReader::ReadDateFromFile(inFile, nvertices);

	m->vertices.resize(nvertices);

	std::string meshName;
	BinaryReader::ReadDateFromFile(inFile, meshName);
	m->name = BinaryReader::stringToWstring(meshName);

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
		else if (token == "<Positions>:")
		{
			int nPositions{};
			BinaryReader::ReadDateFromFile(inFile, nPositions);

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
	m->CreateVertexBufferView(DEVICE, CMDLIST);
	m->CreateIndexBufferView(DEVICE, CMDLIST);

	return m;
}

std::shared_ptr<CMesh> CMesh::CreateCubeMesh(Vec3 scale)
{
	std::shared_ptr<CMesh> m = std::make_shared<CMesh>();

	m->vertices.resize(24);
	m->stride = sizeof(CVertex);
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

void CMesh::CreateVertexBufferView(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	vertexBuffer = CreateBufferResource(device, cmdList, vertices.data(), stride * vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &vertexUploadBuffer);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = stride;
	vertexBufferView.SizeInBytes = stride * vertices.size();
}

void CMesh::CreateIndexBufferView(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	indexBuffer.resize(subMeshNum);
	indexUploadBuffer.resize(subMeshNum);
	indexBufferView.resize(subMeshNum);

	for (int i = 0; i < subMeshNum;i++) {
		indexBuffer[i] = CreateBufferResource(device, cmdList, indices[i].data(),
			sizeof(UINT) * indices[i].size(), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &indexUploadBuffer[i]);

		indexBufferView[i].BufferLocation = indexBuffer[i]->GetGPUVirtualAddress();
		indexBufferView[i].Format = DXGI_FORMAT_R32_UINT;
		indexBufferView[i].SizeInBytes = sizeof(UINT) * indices[i].size();
	}
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
	if(vertexUploadBuffer)
		vertexUploadBuffer.Reset();
	for (auto& idxUploadBuffer : indexUploadBuffer) {
		idxUploadBuffer.Reset();
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
	cmdList->IASetVertexBuffers(slot, 1, &vertexBufferView);

	if (!indices.empty() && indices.size() > curSubSet)
	{
		cmdList->IASetIndexBuffer(&indexBufferView[curSubSet]);
		cmdList->DrawIndexedInstanced(indices[curSubSet].size(), 1, 0, 0, 0);
	}
	else
	{
		cmdList->DrawInstanced(vertices.size(), 1, offset, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* cmdList, int idx)
{
	cmdList->IASetPrimitiveTopology(primitiveTopology);
	cmdList->IASetVertexBuffers(slot, 1, &vertexBufferView);

	if (!indices.empty() && indices.size() > idx)
	{
		cmdList->IASetIndexBuffer(&indexBufferView[idx]);
		cmdList->DrawIndexedInstanced(indices[idx].size(), 1, 0, 0, 0);
	}
	else
	{
		cmdList->DrawInstanced(vertices.size(), 1, offset, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* cmdList, UINT nInstances, const D3D12_VERTEX_BUFFER_VIEW& d3dInstancingBufferView)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[] = { vertexBufferView, d3dInstancingBufferView };
	cmdList->IASetVertexBuffers(slot, _countof(pVertexBufferViews), pVertexBufferViews);
	cmdList->IASetPrimitiveTopology(primitiveTopology);

	Render(cmdList, nInstances);
}

void CMesh::Render(ID3D12GraphicsCommandList* cmdList, UINT nInstances)
{
	cmdList->IASetVertexBuffers(slot, 1, &vertexBufferView);
	cmdList->IASetPrimitiveTopology(primitiveTopology);

	Render(cmdList, nInstances);
}


ID3D12Resource* CreateBufferResource(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
	void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer)
{
	ID3D12Resource* pd3dBuffer = NULL;

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc = CD3DX12_HEAP_PROPERTIES(d3dHeapType);
	D3D12_RESOURCE_DESC d3dResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(nBytes);
	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	ThrowIfFailed(device->CreateCommittedResource(&d3dHeapPropertiesDesc,
		D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, nullptr,
		IID_PPV_ARGS(&pd3dBuffer)));

	if (pData)
	{
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppd3dUploadBuffer)
			{
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				ThrowIfFailed(device->CreateCommittedResource(&d3dHeapPropertiesDesc,
					D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
					__uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer));

				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8* pBufferDataBegin = NULL;

				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);

				memcpy(pBufferDataBegin, pData, nBytes);

				(*ppd3dUploadBuffer)->Unmap(0, NULL);

				cmdList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);

				D3D12_RESOURCE_BARRIER d3dResourceBarrier;
				::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));

				d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				d3dResourceBarrier.Transition.pResource = pd3dBuffer;
				d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
				d3dResourceBarrier.Transition.Subresource =
					D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				cmdList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pd3dBuffer);
}