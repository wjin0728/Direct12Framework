#include "stdafx.h"
#include "GridMesh.h"
#include"DX12Manager.h"
#include"ResourceManager.h"


void CHeightMapGridMesh::LoadHeightMap(const std::wstring& fileName)
{
	UINT heightMapSize = width * height;
	BYTE* pHeightMapPixels = new BYTE[heightMapSize];
	memset(pHeightMapPixels, 0, heightMapSize);

	HANDLE hFile = ::CreateFile(fileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, heightMapSize, &dwBytesRead, NULL);
	::CloseHandle(hFile);

	heightMap = new BYTE[heightMapSize];
	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			heightMap[x + ((height - 1 - y) * width)] = pHeightMapPixels[x +
				(y * width)];
		}
	}

	XMCOLOR* data = new XMCOLOR[heightMapSize];
	for (int i = 0; i < heightMapSize; i++) {
		data[i].a = pHeightMapPixels[i];
		data[i].r = pHeightMapPixels[i];
		data[i].g = pHeightMapPixels[i];
		data[i].b = pHeightMapPixels[i];
	}

	auto heightMapTex = RESOURCE.Create2DTexture(L"heightMap", DXGI_FORMAT_R8G8B8A8_UNORM, data, sizeof(XMCOLOR), width, height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_FLAG_NONE
	);

	heightMapTex->CreateSRV();

	if (pHeightMapPixels) delete[] pHeightMapPixels;

	if (data) delete[] data;
}

void CHeightMapGridMesh::CreateHeightMapSRV()
{
	
}

void CHeightMapGridMesh::CalculateNormal()
{
	std::vector<Vec3> normals(width * height, Vec3::Zero);

	for (UINT i = 0; i < height - 1; ++i) {
		for (UINT j = 0; j < width - 1; ++j) {
			UINT idx0 = (i * width) + j;              //좌측 하단
			UINT idx1 = ((i + 1) * width) + j;		  //좌측 상단
			UINT idx2 = ((i + 1) * width) + (j + 1);  //우측 상단
			UINT idx3 = (i * width) + (j + 1);		  //우측 하단

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
	float increaseVal = static_cast<float>(TEXTURE_REPEAT_COUNT) / width;
	UINT increaseCnt = width / TEXTURE_REPEAT_COUNT;

	Vec2 uv{ 0.f, 1.f };

	UINT uCount{};
	UINT vCount{};

	for (UINT i = 0; i < height; ++i) {
		for (UINT j = 0; j < width; ++j) {
			vertices[(i * width) + j].texCoord = uv;

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

void CHeightMapGridMesh::Initialize(const std::wstring& fileName, int _width, int _height, Vec3 _scale)
{
	width = _width;
	height = _height;
	scale = _scale;

	LoadHeightMap(fileName);

	primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	size_t size = width * height;

	vertices.reserve(size);

	int maxX = width, maxZ = height;

	for (int z = 0; z < maxZ; z++) {
		for (int x = 0; x < maxX; x++) {
			int y = heightMap[x + (z * width)];

			Vec3 position = Vec3(x - maxX / 2.f, y, z - maxZ / 2.f) * scale;
			vertices.emplace_back(position);
		}
	}
	CalculateNormal();
	CalculateTextureCoord();

	CreateVertexBuffer();
}

float CHeightMapGridMesh::GetHeight(float fx, float fz)
{
	fx /= scale.x; fz /= scale.z;

	fx += width / 2.f;
	fz += height / 2.f;


	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= width) || (fz >= height)) {
		return 0.0f;
	}

	int x = (int)fx;
	int z = (int)fz;

	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)heightMap[x + (z * width)];
	float fBottomLocalRight = (float)heightMap[(x + 1) + (z * width)];
	float fTopLeft = (float)heightMap[x + ((z + 1) * width)];
	float fTopRight = (float)heightMap[(x + 1) + ((z + 1) * width)];

	if (fzPercent >= fxPercent)
		fBottomLocalRight = fBottomLeft + (fTopRight - fTopLeft);
	else
		fTopLeft = fTopRight + (fBottomLeft - fBottomLocalRight);

	float fTopHeight = SimpleMath::Flerp(fTopLeft, fTopRight, fxPercent);
	float fBottomHeight = SimpleMath::Flerp(fBottomLeft, fBottomLocalRight, fxPercent);
	float fHeight = SimpleMath::Flerp(fBottomHeight, fTopHeight, fzPercent);

	return fHeight * scale.y;
}
