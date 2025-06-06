#include "stdafx.h"
#include "GridMesh.h"
#include"DX12Manager.h"
#include"ResourceManager.h"


void CHeightMapGridMesh::LoadHeightMap(const std::string& fileName)
{
	UINT heightMapSize = resolution * resolution;

	std::vector<float> heightMap;
	std::ifstream file("..\\Resources\\Textures\\" + fileName + ".raw", std::ios::binary);
	if (!file.is_open()) {
		return;
	}
	heightMap.resize(heightMapSize);
	file.read(reinterpret_cast<char*>(heightMap.data()), heightMapSize * sizeof(float));

	heightData.resize(heightMapSize);
	for (UINT y = 0; y < resolution; y++)
	{
		for (UINT x = 0; x < resolution; x++)
		{
			heightData[x + ((resolution - 1 - y) * resolution)] = heightMap[x + (y * resolution)];
		}
	}

	heightMapTex = std::make_shared<CTexture>(fileName, DXGI_FORMAT_R32_FLOAT, heightData.data(), sizeof(float), resolution, resolution,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_FLAG_NONE
	);
	heightMapTex->SetTextureType(TEXTURE2D);
	RESOURCE.ProcessGPULoadImmediate(heightMapTex.get());
	RESOURCE.Add(heightMapTex);
	heightMapTex->AssignedSRVIndex();
	heightMapTex->CreateSRV();
}

void CHeightMapGridMesh::CreateHeightMapSRV()
{
	
}

void CHeightMapGridMesh::CalculateNormal()
{
	std::vector<Vec3> normals(resolution * resolution, Vec3::Zero);

	for (UINT i = 0; i < resolution - 1; ++i) {
		for (UINT j = 0; j < resolution - 1; ++j) {
			UINT idx0 = (i * resolution) + j;              //좌측 하단
			UINT idx1 = ((i + 1) * resolution) + j;		  //좌측 상단
			UINT idx2 = ((i + 1) * resolution) + (j + 1);  //우측 상단
			UINT idx3 = (i * resolution) + (j + 1);		  //우측 하단

			Vec3 edge1 = vertices[idx1].position - vertices[idx0].position;
			Vec3 edge2 = vertices[idx2].position - vertices[idx0].position;
			Vec3 edge3 = vertices[idx3].position - vertices[idx0].position;

			Vec3 normal1 = edge1.Cross(edge2);
			normals[idx0] += normal1;
			normals[idx1] += normal1;
			normals[idx2] += normal1;

			Vec3 normal2 = edge2.Cross(edge3);
			normals[idx0] += normal2;
			normals[idx2] += normal2;
			normals[idx3] += normal2;
		}
	}
	std::transform(normals.begin(), normals.end(), normals.begin(), [](Vec3& n) { return n.GetNormalized(); });

	for (size_t  i = 0; i < vertices.size(); ++i) {
		vertices[i].normal = normals[i];
	}
}

void CHeightMapGridMesh::CalculateTextureCoord()
{
	float increaseVal = static_cast<float>(TEXTURE_REPEAT_COUNT) / resolution;
	UINT increaseCnt = resolution / TEXTURE_REPEAT_COUNT;

	Vec2 uv{ 0.f, 1.f };

	UINT uCount{};
	UINT vCount{};

	for (UINT i = 0; i < resolution; ++i) {
		for (UINT j = 0; j < resolution; ++j) {
			vertices[(i * resolution) + j].texCoord = uv;

			uv.x += increaseVal;
			uCount++;

			if (uCount == increaseCnt) {
				uv.x = 0.f;
				uCount = 0;
			}
		}
		uv.y -= increaseVal;
		vCount++;

		if (vCount == increaseCnt) {
			uv.y = 1.f;
			vCount = 0;
		}
	}
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

void CHeightMapGridMesh::Initialize(const std::string& fileName, UINT _resoultion, Vec3 _scale, Vec3 offset)
{
	resolution = _resoultion;
	scale = _scale;
	this->offset = offset;

	LoadHeightMap(fileName);

	primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	size_t size = resolution * resolution;

	vertices.reserve(size);

	float xGap = scale.x / resolution;
	float zGap = scale.z / resolution;

	for (int i = 0; i < resolution; i++) {
		for (int j = 0; j < resolution; j++) {
			float x = j * xGap;
			float z = i * zGap;

			float y = heightData[j + (i * resolution)] * scale.y;

			Vec3 position = Vec3{ x,y,z } + offset;
			vertices.emplace_back(position);
		}
	}
	CalculateNormal();
	CalculateTextureCoord();
}

float CHeightMapGridMesh::GetHeight(float fx, float fz)
{
	float localX = fx - offset.x;
	float localZ = fz - offset.z;

	if (localX < 0.0f || localZ < 0.0f || localX >= scale.x || localZ >= scale.z) {
		return 0.0f;
	}

	float xIndex = localX / (scale.x / (resolution));
	float zIndex = localZ / (scale.z / (resolution));
	zIndex = resolution - zIndex;


	int x = static_cast<int>(xIndex);
	int z = static_cast<int>(zIndex);

	float fxPercent = xIndex - x;
	float fzPercent = zIndex - z;

	float fBottomLeft = heightData[x + (z * resolution)] * scale.y;
	float fBottomRight = heightData[(x + 1) + (z * resolution)] * scale.y;
	float fTopLeft = heightData[x + ((z + 1) * resolution)] * scale.y;
	float fTopRight = heightData[(x + 1) + ((z + 1) * resolution)] * scale.y;

	if (fzPercent >= fxPercent) {
		fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
	}
	else {
		fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}

	float fTopHeight = SimpleMath::Flerp(fTopLeft, fTopRight, fxPercent);
	float fBottomHeight = SimpleMath::Flerp(fBottomLeft, fBottomRight, fxPercent);
	float fHeight = SimpleMath::Flerp(fBottomHeight, fTopHeight, fzPercent);

	return fHeight;
}
