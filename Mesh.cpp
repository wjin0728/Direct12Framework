#include "stdafx.h"
#include "Mesh.h"
#include<numeric>
#include"DX12Manager.h"


CVertex::CVertex(const XMFLOAT3& pos, const XMFLOAT3& nor) : normal(nor)
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
			XMFLOAT3 boundingCenter, boundingExtent;

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

	int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0;
	int nvertices = 0;

	BinaryReader::ReadDateFromFile(inFile, nvertices);

	//vertices.resize(nvertices);

	std::string meshName;
	BinaryReader::ReadDateFromFile(inFile, meshName);

	std::string token;

	for (; ; )
	{
		BinaryReader::ReadDateFromFile(inFile, token);

		if (token == "<Bounds>:")
		{
			XMFLOAT3 boundingCenter, boundingExtent;

			BinaryReader::ReadDateFromFile(inFile, boundingCenter);
			BinaryReader::ReadDateFromFile(inFile, boundingExtent);

			BoundingOrientedBox::CreateFromBoundingBox(m->oobb, BoundingBox(boundingCenter, boundingExtent));
			BoundingSphere::CreateFromBoundingBox(m->oobs, m->oobb);
		}
		else if (token == "<Positions>:")
		{
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
			BinaryReader::ReadDateFromFile(inFile, nNormals);

			for (int i = 0; i < nNormals; i++) {
				BinaryReader::ReadDateFromFile(inFile, m->vertices[i].normal);
			}
		}
		else if (token == "<Indices>:")
		{
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

	return m;
}

std::shared_ptr<CMesh> CMesh::CreateCubeMesh(XMFLOAT3 scale)
{
	std::shared_ptr<CMesh> m = std::make_shared<CMesh>();

	m->vertices.resize(24);
	m->stride = sizeof(CVertex);
	m->primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = scale.x * 0.5f, fy = scale.y * 0.5f, fz = scale.z * 0.5f;

	//¾Õ¸é
	m->vertices[0] = CVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	m->vertices[1] = CVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	m->vertices[2] = CVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	m->vertices[3] = CVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	// µÞ¸é
	m->vertices[4] = CVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m->vertices[5] = CVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m->vertices[6] = CVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m->vertices[7] = CVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
	// À­¸é
	m->vertices[8] = CVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	m->vertices[9] = CVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	m->vertices[10] = CVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	m->vertices[11] = CVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	// ¾Æ·§¸é			
	m->vertices[12] = CVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m->vertices[13] = CVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m->vertices[14] = CVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m->vertices[15] = CVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
	// ¿ÞÂÊ¸é							
	m->vertices[16] = CVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
	m->vertices[17] = CVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
	m->vertices[18] = CVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
	m->vertices[19] = CVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
	// ¿À¸¥ÂÊ¸é		   					
	m->vertices[20] = CVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
	m->vertices[21] = CVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
	m->vertices[22] = CVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
	m->vertices[23] = CVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));

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

std::shared_ptr<CMesh> CMesh::CreateAlphabetMesh(char type, XMFLOAT3 scale)
{
	std::shared_ptr<CMesh> m = std::make_shared<CMesh>();

	float fx = scale.x * 0.5f, fy = scale.y * 0.5f, fz = scale.z * 0.5f;
	int i = 0;

	m->primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	switch (type)
	{
	case 'S':
	{
		int nCube = 5;
		m->vertices.reserve(8 * nCube);
		m->indices.emplace_back(36 * nCube);

		float offSet = fy * 0.2f;
		float fHalfDepth = fz;

		float right[5];
		float top[5];
		float left[5];
		float bottom[5];

		int idx = 0;

		right[idx] = fx;
		top[idx] = fy;
		left[idx] = -fx;
		bottom[idx] = fy - offSet;
		idx++;

		right[idx] = -fx + offSet;
		top[idx] = fy;
		left[idx] = -fx;
		bottom[idx] = offSet * 0.5f;
		idx++;

		right[idx] = fx;
		top[idx] = offSet * 0.5f;
		left[idx] = -fx;
		bottom[idx] = offSet * -0.5f;
		idx++;

		right[idx] = fx;
		top[idx] = offSet * 0.5f;
		left[idx] = fx - offSet;
		bottom[idx] = -fy;
		idx++;

		right[idx] = fx;
		top[idx] = -fy + offSet;
		left[idx] = -fx;
		bottom[idx] = -fy;
		idx++;

		for (int i = 0; i < nCube; ++i) {
			m->vertices.emplace_back(XMFLOAT3(left[i], top[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, +1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], top[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, +1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], top[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, +1.f, +1.f)));
			m->vertices.emplace_back(XMFLOAT3(left[i], top[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, +1.f, +1.f)));
			m->vertices.emplace_back(XMFLOAT3(left[i], bottom[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, -1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], bottom[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, -1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], bottom[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, -1.f, +1.f)));
			m->vertices.emplace_back(XMFLOAT3(left[i], bottom[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, -1.f, +1.f)));

			int stride = i * 36;
			int vStride = i * 8;

			m->indices[0][0 + stride] = 3 + vStride; m->indices[0][1 + stride] = 1 + vStride; m->indices[0][2 + stride] = 0 + vStride;
			m->indices[0][3 + stride] = 2 + vStride; m->indices[0][4 + stride] = 1 + vStride; m->indices[0][5 + stride] = 3 + vStride;
			m->indices[0][6 + stride] = 0 + vStride; m->indices[0][7 + stride] = 5 + vStride; m->indices[0][8 + stride] = 4 + vStride;
			m->indices[0][9 + stride] = 1 + vStride; m->indices[0][10 + stride] = 5 + vStride; m->indices[0][11 + stride] = 0 + vStride;
			m->indices[0][12 + stride] = 3 + vStride; m->indices[0][13 + stride] = 4 + vStride; m->indices[0][14 + stride] = 7 + vStride;
			m->indices[0][15 + stride] = 0 + vStride; m->indices[0][16 + stride] = 4 + vStride; m->indices[0][17 + stride] = 3 + vStride;
			m->indices[0][18 + stride] = 1 + vStride; m->indices[0][19 + stride] = 6 + vStride; m->indices[0][20 + stride] = 5 + vStride;
			m->indices[0][21 + stride] = 2 + vStride; m->indices[0][22 + stride] = 6 + vStride; m->indices[0][23 + stride] = 1 + vStride;
			m->indices[0][24 + stride] = 2 + vStride; m->indices[0][25 + stride] = 7 + vStride; m->indices[0][26 + stride] = 6 + vStride;
			m->indices[0][27 + stride] = 3 + vStride; m->indices[0][28 + stride] = 7 + vStride; m->indices[0][29 + stride] = 2 + vStride;
			m->indices[0][30 + stride] = 6 + vStride; m->indices[0][31 + stride] = 4 + vStride; m->indices[0][32 + stride] = 5 + vStride;
			m->indices[0][33 + stride] = 7 + vStride; m->indices[0][34 + stride] = 4 + vStride; m->indices[0][35 + stride] = 6 + vStride;
		}

		break;
	}
	case 'T':
	{

		float offSet = fy * 0.2f;
		float fHalfDepth = fz;

		int nCube = 2;
		m->vertices.reserve(8 * nCube);
		m->indices.emplace_back(36 * nCube);

		float right[2];
		float top[2];
		float left[2];
		float bottom[2];

		int idx = 0;

		right[idx] = fx;
		top[idx] = fy;
		left[idx] = -fx;
		bottom[idx] = fy - offSet;
		idx++;

		right[idx] = offSet * 0.5f;
		top[idx] = fy;
		left[idx] = offSet * -0.5f;
		bottom[idx] = -fy;
		idx++;

		for (int i = 0; i < nCube; ++i) {
			m->vertices.emplace_back(XMFLOAT3(left[i], top[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, +1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], top[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, +1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], top[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, +1.f, +1.f)));
			m->vertices.emplace_back(XMFLOAT3(left[i], top[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, +1.f, +1.f)));
			m->vertices.emplace_back(XMFLOAT3(left[i], bottom[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, -1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], bottom[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, -1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], bottom[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, -1.f, +1.f)));
			m->vertices.emplace_back(XMFLOAT3(left[i], bottom[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, -1.f, +1.f)));

			int stride = i * 36;
			int vStride = i * 8;

			m->indices[0][0 + stride] = 3 + vStride; m->indices[0][1 + stride] = 1 + vStride; m->indices[0][2 + stride] = 0 + vStride;
			m->indices[0][3 + stride] = 2 + vStride; m->indices[0][4 + stride] = 1 + vStride; m->indices[0][5 + stride] = 3 + vStride;
			m->indices[0][6 + stride] = 0 + vStride; m->indices[0][7 + stride] = 5 + vStride; m->indices[0][8 + stride] = 4 + vStride;
			m->indices[0][9 + stride] = 1 + vStride; m->indices[0][10 + stride] = 5 + vStride; m->indices[0][11 + stride] = 0 + vStride;
			m->indices[0][12 + stride] = 3 + vStride; m->indices[0][13 + stride] = 4 + vStride; m->indices[0][14 + stride] = 7 + vStride;
			m->indices[0][15 + stride] = 0 + vStride; m->indices[0][16 + stride] = 4 + vStride; m->indices[0][17 + stride] = 3 + vStride;
			m->indices[0][18 + stride] = 1 + vStride; m->indices[0][19 + stride] = 6 + vStride; m->indices[0][20 + stride] = 5 + vStride;
			m->indices[0][21 + stride] = 2 + vStride; m->indices[0][22 + stride] = 6 + vStride; m->indices[0][23 + stride] = 1 + vStride;
			m->indices[0][24 + stride] = 2 + vStride; m->indices[0][25 + stride] = 7 + vStride; m->indices[0][26 + stride] = 6 + vStride;
			m->indices[0][27 + stride] = 3 + vStride; m->indices[0][28 + stride] = 7 + vStride; m->indices[0][29 + stride] = 2 + vStride;
			m->indices[0][30 + stride] = 6 + vStride; m->indices[0][31 + stride] = 4 + vStride; m->indices[0][32 + stride] = 5 + vStride;
			m->indices[0][33 + stride] = 7 + vStride; m->indices[0][34 + stride] = 4 + vStride; m->indices[0][35 + stride] = 6 + vStride;
		}
		break;
	}
	case 'A':
	{
		float offSet = fy * 0.15f;
		float fHalfDepth = fz;

		int nCube = 3;
		m->vertices.reserve(8 * nCube);
		m->indices.emplace_back(36 * nCube);

		XMFLOAT3 pos[3][8];

		int idx = 0;

		pos[idx][0] = { offSet * -0.5f, fy, -fHalfDepth };
		pos[idx][1] = { offSet * 0.5f, fy, -fHalfDepth };
		pos[idx][2] = { offSet * 0.5f, fy, +fHalfDepth };
		pos[idx][3] = { offSet * -0.5f, fy, +fHalfDepth };
		pos[idx][4] = { fx - offSet, -fy, -fHalfDepth };
		pos[idx][5] = { fx, -fy, -fHalfDepth };
		pos[idx][6] = { fx, -fy, +fHalfDepth };
		pos[idx][7] = { fx - offSet, -fy, +fHalfDepth };
		idx++;

		pos[idx][0] = { offSet * -0.5f, fy, -fHalfDepth };
		pos[idx][1] = { offSet * 0.5f, fy, -fHalfDepth };
		pos[idx][2] = { offSet * 0.5f, fy, +fHalfDepth };
		pos[idx][3] = { offSet * -0.5f, fy, +fHalfDepth };
		pos[idx][4] = { -fx, -fy, -fHalfDepth };
		pos[idx][5] = { -fx + offSet, -fy, -fHalfDepth };
		pos[idx][6] = { -fx + offSet, -fy, +fHalfDepth };
		pos[idx][7] = { -fx, -fy, +fHalfDepth };
		idx++;

		XMFLOAT3 dir = Vector3::Subtract(XMFLOAT3(offSet * -0.5f, fy, -fHalfDepth), XMFLOAT3(fx - offSet, -fy, -fHalfDepth));
		dir = Vector3::Normalize(dir);
		XMFLOAT3 bottom = Vector3::VectorLerp(XMFLOAT3(offSet * -0.5f, fy, -fHalfDepth), XMFLOAT3(fx - offSet, -fy, -fHalfDepth), 0.6f);
		XMFLOAT3 top = Vector3::Add(bottom, dir, offSet);

		pos[idx][0] = { -top.x, top.y, -fHalfDepth };
		pos[idx][1] = { +top.x, top.y, -fHalfDepth };
		pos[idx][2] = { +top.x, top.y, +fHalfDepth };
		pos[idx][3] = { -top.x, top.y, +fHalfDepth };
		pos[idx][4] = { -bottom.x, bottom.y, -fHalfDepth };
		pos[idx][5] = { +bottom.x, bottom.y, -fHalfDepth };
		pos[idx][6] = { +bottom.x, bottom.y, +fHalfDepth };
		pos[idx][7] = { -bottom.x, bottom.y, +fHalfDepth };
		idx++;

		for (int i = 0; i < nCube; ++i)
		{
			m->vertices.emplace_back(pos[i][0], Vector3::Normalize(XMFLOAT3(-1.f, +1.f, -1.f)));
			m->vertices.emplace_back(pos[i][1], Vector3::Normalize(XMFLOAT3(+1.f, +1.f, -1.f)));
			m->vertices.emplace_back(pos[i][2], Vector3::Normalize(XMFLOAT3(+1.f, +1.f, +1.f)));
			m->vertices.emplace_back(pos[i][3], Vector3::Normalize(XMFLOAT3(-1.f, +1.f, +1.f)));
			m->vertices.emplace_back(pos[i][4], Vector3::Normalize(XMFLOAT3(-1.f, -1.f, -1.f)));
			m->vertices.emplace_back(pos[i][5], Vector3::Normalize(XMFLOAT3(+1.f, -1.f, -1.f)));
			m->vertices.emplace_back(pos[i][6], Vector3::Normalize(XMFLOAT3(+1.f, -1.f, +1.f)));
			m->vertices.emplace_back(pos[i][7], Vector3::Normalize(XMFLOAT3(-1.f, -1.f, +1.f)));

			int stride = i * 36;
			int vStride = i * 8;

			m->indices[0][0 + stride] = 3 + vStride; m->indices[0][1 + stride] = 1 + vStride; m->indices[0][2 + stride] = 0 + vStride;
			m->indices[0][3 + stride] = 2 + vStride; m->indices[0][4 + stride] = 1 + vStride; m->indices[0][5 + stride] = 3 + vStride;
			m->indices[0][6 + stride] = 0 + vStride; m->indices[0][7 + stride] = 5 + vStride; m->indices[0][8 + stride] = 4 + vStride;
			m->indices[0][9 + stride] = 1 + vStride; m->indices[0][10 + stride] = 5 + vStride; m->indices[0][11 + stride] = 0 + vStride;
			m->indices[0][12 + stride] = 3 + vStride; m->indices[0][13 + stride] = 4 + vStride; m->indices[0][14 + stride] = 7 + vStride;
			m->indices[0][15 + stride] = 0 + vStride; m->indices[0][16 + stride] = 4 + vStride; m->indices[0][17 + stride] = 3 + vStride;
			m->indices[0][18 + stride] = 1 + vStride; m->indices[0][19 + stride] = 6 + vStride; m->indices[0][20 + stride] = 5 + vStride;
			m->indices[0][21 + stride] = 2 + vStride; m->indices[0][22 + stride] = 6 + vStride; m->indices[0][23 + stride] = 1 + vStride;
			m->indices[0][24 + stride] = 2 + vStride; m->indices[0][25 + stride] = 7 + vStride; m->indices[0][26 + stride] = 6 + vStride;
			m->indices[0][27 + stride] = 3 + vStride; m->indices[0][28 + stride] = 7 + vStride; m->indices[0][29 + stride] = 2 + vStride;
			m->indices[0][30 + stride] = 6 + vStride; m->indices[0][31 + stride] = 4 + vStride; m->indices[0][32 + stride] = 5 + vStride;
			m->indices[0][33 + stride] = 7 + vStride; m->indices[0][34 + stride] = 4 + vStride; m->indices[0][35 + stride] = 6 + vStride;
		}

		break;
	}
	case 'R':
	{

		float offSet = fy * 0.2f;
		float fHalfDepth = fz;

		int nCube = 5;
		m->vertices.reserve(8 * nCube);
		m->indices.emplace_back(36 * nCube);

		float right[5];
		float top[5];
		float left[5];
		float bottom[5];

		int idx = 0;

		right[idx] = fx;
		top[idx] = fy;
		left[idx] = -fx;
		bottom[idx] = fy - offSet;
		idx++;

		right[idx] = -fx + offSet;
		top[idx] = fy;
		left[idx] = -fx;
		bottom[idx] = -fy;
		idx++;

		right[idx] = fx;
		top[idx] = offSet * 0.5f;
		left[idx] = -fx;
		bottom[idx] = offSet * -0.5f;
		idx++;

		right[idx] = fx;
		top[idx] = fy;
		left[idx] = fx - offSet;
		bottom[idx] = offSet * -0.5f;
		idx++;

		left[idx] = -fx + offSet;
		right[idx] = left[idx - 1] + offSet;
		top[idx] = offSet * -0.5f;
		bottom[idx] = -fy;
		idx++;

		XMFLOAT3 pos[8];
		pos[0] = { -fx + offSet, offSet * -0.5f, -fHalfDepth };
		pos[1] = { -fx + offSet * 2, offSet * -0.5f, -fHalfDepth };
		pos[2] = { -fx + offSet * 2, offSet * -0.5f, +fHalfDepth };
		pos[3] = { -fx + offSet, offSet * -0.5f, +fHalfDepth };
		pos[4] = { fx - offSet, -fy, -fHalfDepth };
		pos[5] = { fx, -fy, -fHalfDepth };
		pos[6] = { fx, -fy, +fHalfDepth };
		pos[7] = { fx - offSet, -fy, +fHalfDepth };

		for (int i = 0; i < nCube - 1; ++i) {
			m->vertices.emplace_back(XMFLOAT3(left[i], top[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, +1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], top[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, +1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], top[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, +1.f, +1.f)));
			m->vertices.emplace_back(XMFLOAT3(left[i], top[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, +1.f, +1.f)));
			m->vertices.emplace_back(XMFLOAT3(left[i], bottom[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, -1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], bottom[i], -fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, -1.f, -1.f)));
			m->vertices.emplace_back(XMFLOAT3(right[i], bottom[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(+1.f, -1.f, +1.f)));
			m->vertices.emplace_back(XMFLOAT3(left[i], bottom[i], +fHalfDepth), Vector3::Normalize(XMFLOAT3(-1.f, -1.f, +1.f)));

			int stride = i * 36;
			int vStride = i * 8;

			m->indices[0][0 + stride] = 3 + vStride; m->indices[0][1 + stride] = 1 + vStride; m->indices[0][2 + stride] = 0 + vStride;
			m->indices[0][3 + stride] = 2 + vStride; m->indices[0][4 + stride] = 1 + vStride; m->indices[0][5 + stride] = 3 + vStride;
			m->indices[0][6 + stride] = 0 + vStride; m->indices[0][7 + stride] = 5 + vStride; m->indices[0][8 + stride] = 4 + vStride;
			m->indices[0][9 + stride] = 1 + vStride; m->indices[0][10 + stride] = 5 + vStride; m->indices[0][11 + stride] = 0 + vStride;
			m->indices[0][12 + stride] = 3 + vStride; m->indices[0][13 + stride] = 4 + vStride; m->indices[0][14 + stride] = 7 + vStride;
			m->indices[0][15 + stride] = 0 + vStride; m->indices[0][16 + stride] = 4 + vStride; m->indices[0][17 + stride] = 3 + vStride;
			m->indices[0][18 + stride] = 1 + vStride; m->indices[0][19 + stride] = 6 + vStride; m->indices[0][20 + stride] = 5 + vStride;
			m->indices[0][21 + stride] = 2 + vStride; m->indices[0][22 + stride] = 6 + vStride; m->indices[0][23 + stride] = 1 + vStride;
			m->indices[0][24 + stride] = 2 + vStride; m->indices[0][25 + stride] = 7 + vStride; m->indices[0][26 + stride] = 6 + vStride;
			m->indices[0][27 + stride] = 3 + vStride; m->indices[0][28 + stride] = 7 + vStride; m->indices[0][29 + stride] = 2 + vStride;
			m->indices[0][30 + stride] = 6 + vStride; m->indices[0][31 + stride] = 4 + vStride; m->indices[0][32 + stride] = 5 + vStride;
			m->indices[0][33 + stride] = 7 + vStride; m->indices[0][34 + stride] = 4 + vStride; m->indices[0][35 + stride] = 6 + vStride;
		}

		int stride = 4 * 36;
		int vStride = 4 * 8;

		m->vertices.emplace_back(pos[0], Vector3::Normalize(XMFLOAT3(-1.f, +1.f, -1.f)));
		m->vertices.emplace_back(pos[1], Vector3::Normalize(XMFLOAT3(+1.f, +1.f, -1.f)));
		m->vertices.emplace_back(pos[2], Vector3::Normalize(XMFLOAT3(+1.f, +1.f, +1.f)));
		m->vertices.emplace_back(pos[3], Vector3::Normalize(XMFLOAT3(-1.f, +1.f, +1.f)));
		m->vertices.emplace_back(pos[4], Vector3::Normalize(XMFLOAT3(-1.f, -1.f, -1.f)));
		m->vertices.emplace_back(pos[5], Vector3::Normalize(XMFLOAT3(+1.f, -1.f, -1.f)));
		m->vertices.emplace_back(pos[6], Vector3::Normalize(XMFLOAT3(+1.f, -1.f, +1.f)));
		m->vertices.emplace_back(pos[7], Vector3::Normalize(XMFLOAT3(-1.f, -1.f, +1.f)));

		m->indices[0][0 + stride] = 3 + vStride; m->indices[0][1 + stride] = 1 + vStride; m->indices[0][2 + stride] = 0 + vStride;
		m->indices[0][3 + stride] = 2 + vStride; m->indices[0][4 + stride] = 1 + vStride; m->indices[0][5 + stride] = 3 + vStride;
		m->indices[0][6 + stride] = 0 + vStride; m->indices[0][7 + stride] = 5 + vStride; m->indices[0][8 + stride] = 4 + vStride;
		m->indices[0][9 + stride] = 1 + vStride; m->indices[0][10 + stride] = 5 + vStride; m->indices[0][11 + stride] = 0 + vStride;
		m->indices[0][12 + stride] = 3 + vStride; m->indices[0][13 + stride] = 4 + vStride; m->indices[0][14 + stride] = 7 + vStride;
		m->indices[0][15 + stride] = 0 + vStride; m->indices[0][16 + stride] = 4 + vStride; m->indices[0][17 + stride] = 3 + vStride;
		m->indices[0][18 + stride] = 1 + vStride; m->indices[0][19 + stride] = 6 + vStride; m->indices[0][20 + stride] = 5 + vStride;
		m->indices[0][21 + stride] = 2 + vStride; m->indices[0][22 + stride] = 6 + vStride; m->indices[0][23 + stride] = 1 + vStride;
		m->indices[0][24 + stride] = 2 + vStride; m->indices[0][25 + stride] = 7 + vStride; m->indices[0][26 + stride] = 6 + vStride;
		m->indices[0][27 + stride] = 3 + vStride; m->indices[0][28 + stride] = 7 + vStride; m->indices[0][29 + stride] = 2 + vStride;
		m->indices[0][30 + stride] = 6 + vStride; m->indices[0][31 + stride] = 4 + vStride; m->indices[0][32 + stride] = 5 + vStride;
		m->indices[0][33 + stride] = 7 + vStride; m->indices[0][34 + stride] = 4 + vStride; m->indices[0][35 + stride] = 6 + vStride;

		break;
	}
	default:
		break;
	}

	m->CalculateNormal();
	m->CreateOOBBAndOOBS();

	return m;
}

std::shared_ptr<CMesh> CMesh::CreateRectangleMesh(XMFLOAT2 scale)
{
	std::shared_ptr<CMesh> m = std::make_shared<CMesh>();

	float w2 = 0.5f * scale.x;
	float fy = 0.5f * scale.y;

	m->vertices.resize(4);

	// ¾Õ¸é
	m->vertices[0] = CVertex(XMFLOAT3(-w2, -fy, 0), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	m->vertices[1] = CVertex(XMFLOAT3(-w2, +fy, 0), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	m->vertices[2] = CVertex(XMFLOAT3(+w2, +fy, 0), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	m->vertices[3] = CVertex(XMFLOAT3(+w2, -fy, 0), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));

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

	v.position = XMFLOAT3(0.0f, radius, 0.0f);
	v.texCoord = XMFLOAT2(0.5f, 0.0f);
	v.normal = Vector3::Normalize(v.position);
	v.tangent = XMFLOAT3(1.0f, 0.0f, 1.0f);

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

			v.texCoord = XMFLOAT2(deltaU * x, deltaV * y);

			v.normal = Vector3::Normalize(v.position);

			v.tangent.x = -radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sinf(phi) * cosf(theta);
			v.tangent = Vector3::Normalize(v.tangent);

			m->vertices.push_back(v);
		}
	}

	v.position = XMFLOAT3(0.0f, -radius, 0.0f);
	v.texCoord = XMFLOAT2(0.5f, 1.0f);
	v.normal = Vector3::Normalize(v.position);
	v.tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

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
	BoundingOrientedBox::CreateFromPoints(oobb, vertices.size(), (XMFLOAT3*)vertices.data(), sizeof(CVertex));
	BoundingSphere::CreateFromPoints(oobs, vertices.size(), (XMFLOAT3*)vertices.data(), sizeof(CVertex));
}

void CMesh::CalculateNormal()
{
	for (auto& v : vertices) {
		v.normal = XMFLOAT3(0.f, 0.f, 0.f);
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

		XMFLOAT3 v0 = vertices[v0Idx].position;
		XMFLOAT3 v1 = vertices[v1Idx].position;
		XMFLOAT3 v2 = vertices[v2Idx].position;

		XMFLOAT3 edge1 = v1 - v0;
		XMFLOAT3 edge2 = v2 - v0;

		XMFLOAT3 normal = Vector3::CrossProduct(edge1, edge2, true);

		vertices[v0Idx].normal = Vector3::Normalize(vertices[v0Idx].normal + normal);
		vertices[v1Idx].normal = Vector3::Normalize(vertices[v1Idx].normal + normal);
		vertices[v2Idx].normal = Vector3::Normalize(vertices[v2Idx].normal + normal);
	}
}

void CMesh::ReleaseUploadBuffers()
{
	vertexBuffer.Reset();
	for(auto& idxBuffer : indexBuffer)
		idxBuffer.Reset();
}

BOOL CMesh::RayIntersectionByTriangle(const XMFLOAT3& xmRayOrigin, const XMFLOAT3& xmRayDirection, 
	const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2, float* pfNearHitDistance)
{
	float fHitDistance;
	BOOL bIntersected = Triangle::Intersect(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;

	return(bIntersected);
}

int CMesh::CheckRayIntersection(const XMFLOAT3& xmvPickRayOrigin, const XMFLOAT3& xmvPickRayDirection, float* pfNearHitDistance)
{
	int nIntersections = 0;
	int nOffset = (primitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;

	UINT nVertices = vertices.size();

	int nPrimitives = (primitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ?
		(nVertices / 3) : (nVertices - 2);

	UINT nIndices = indices[curSubSet].size();

	if (nIndices) nPrimitives = (primitiveTopology ==
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (nIndices / 3) : (nIndices - 2);

	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmvPickRayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmvPickRayDirection);

	bool bIntersected = oobb.Intersects(xmRayOrigin, xmRayDirection,
		*pfNearHitDistance);

	if (bIntersected)
	{
		float fNearHitDistance = FLT_MAX;

		for (int i = 0; i < nPrimitives; i++)
		{
			UINT v0Idx = ((nIndices) ? (indices[curSubSet][(i * nOffset) + 0]) : ((i * nOffset) + 0));
			UINT v1Idx = ((nIndices) ? (indices[curSubSet][(i * nOffset) + 1]) : ((i * nOffset) + 1));
			UINT v2Idx = ((nIndices) ? (indices[curSubSet][(i * nOffset) + 2]) : ((i * nOffset) + 2));

			XMVECTOR v0 = XMLoadFloat3(&(vertices[v0Idx].position));
			XMVECTOR v1 = XMLoadFloat3(&(vertices[v1Idx].position));
			XMVECTOR v2 = XMLoadFloat3(&(vertices[v2Idx].position));

			float fHitDistance;
			BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0,
				v1, v2, fHitDistance);

			if (bIntersected)
			{
				if (fHitDistance < fNearHitDistance)
				{
					*pfNearHitDistance = fNearHitDistance = fHitDistance;
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