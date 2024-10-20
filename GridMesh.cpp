#include "stdafx.h"
#include "GridMesh.h"
#include"HeightMapImage.h"


XMFLOAT3 CHeightMapGridMesh::CalculateNormal(int x, int z, CHeightMap* heightMapImage)
{
	XMFLOAT3 normal = { 0.f,0.f,0.f };

	std::queue<XMFLOAT3> normals;

	if ((x < (width - 1)) && (z < (height - 1))) 
		normals.push(heightMapImage->GetHeightMapNormal(x, z));
	if ((z > 0) && (x < (width - 1))) 
		normals.push(heightMapImage->GetHeightMapNormal(x, z - 1));

	XMFLOAT3 scale = heightMapImage->GetScale();

	const BYTE* heightMapPixels = heightMapImage->GetHeightMapPixels();

	if (z & 1) 
	{
		if ((x > 0) && (z < (height - 1)))
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x - 1 + width * z] * scale.y;
			float y3 = (float)heightMapPixels[x + width * (z + 1)] * scale.y;

			XMFLOAT3 edge1 = { -scale.x, y2 - y1, 0.f };
			XMFLOAT3 edge2 = { 0.f, y3 - y1, scale.z };

			normals.push(Vector3::CrossProduct(edge1, edge2));
		}
		if ((x > 0) && (z > 0))
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x + width * (z - 1)] * scale.y;
			float y3 = (float)heightMapPixels[x - 1 + width * z] * scale.y;

			XMFLOAT3 edge1 = { 0.f, y2 - y1, -scale.z };
			XMFLOAT3 edge2 = { -scale.x, y3 - y1, 0.f };

			normals.push(Vector3::CrossProduct(edge1, edge2));
		}
		if ((x < (width - 1)) && (z < (height - 1)))
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x + 1 + width * (z + 1)] * scale.y;
			float y3 = (float)heightMapPixels[x + 1 + width * z] * scale.y;

			XMFLOAT3 edge1 = { scale.x, y2 - y1, scale.z };
			XMFLOAT3 edge2 = { scale.x, y3 - y1, 0 };

			normals.push(Vector3::CrossProduct(edge1, edge2));
		}
		if ((x < (width - 1)) && (z > 0))
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x + 1 + width * z] * scale.y;
			float y3 = (float)heightMapPixels[x + 1 + width * (z - 1)] * scale.y;

			XMFLOAT3 edge1 = { scale.x, y2 - y1, 0.f };
			XMFLOAT3 edge2 = { scale.x, y3 - y1, -scale.z };

			normals.push(Vector3::CrossProduct(edge1, edge2));
		}
	}
	else 
	{
		if (x > 0 && (z < (height - 1))) 
			normals.push(heightMapImage->GetHeightMapNormal(x - 1, z));
		if ((z > 0) && (x > 0)) 
			normals.push(heightMapImage->GetHeightMapNormal(x - 1, z - 1));

		if ((x > 0) && (z < (height - 1))) 
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x - 1 + width * (z + 1)] * scale.y;
			float y3 = (float)heightMapPixels[x + width * (z + 1)] * scale.y;

			XMFLOAT3 edge1 = { -scale.x, y2 - y1, scale.z };
			XMFLOAT3 edge2 = { 0.f, y3 - y1, scale.z };

			normals.push(Vector3::CrossProduct(edge1, edge2));
		}
		if ((x > 0) && (z > 0))
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x + width * (z - 1)] * scale.y;
			float y3 = (float)heightMapPixels[x - 1 + width * (z - 1)] * scale.y;

			XMFLOAT3 edge1 = { -scale.x, y2 - y1, -scale.z };
			XMFLOAT3 edge2 = { 0.f, y3 - y1, -scale.z };

			normals.push(Vector3::CrossProduct(edge1, edge2));
		}
	}
	while (!normals.empty())
	{
		normal = normal + normals.front();
		normals.pop();
	}
	return normal;
}

CHeightMapGridMesh::CHeightMapGridMesh(int xStart, int zStart, int nWidth, int nLength, CHeightMap* heightMapImage)
{
	width = nWidth;
	height = nLength;

	stride = sizeof(CVertex);

	primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	size_t size = width * height;

	vertices.reserve(size);

	
	int maxX = xStart + width, maxZ = zStart + height;

	XMFLOAT3 scale = heightMapImage->GetScale();
	const BYTE* heightMapPixels = heightMapImage->GetHeightMapPixels();

	for (int z = zStart; z < maxZ; z++) {
		for (int x = xStart; x < maxX; x++) {
			int y = heightMapPixels[x + (z * nWidth)];

			XMFLOAT3 position = XMFLOAT3( x - maxX/2, y, z - maxZ/2) * scale;
			XMFLOAT3 normal = CalculateNormal(x, z, heightMapImage);
			vertices.emplace_back(position, normal);
		}
	}

	UINT nIndices = ((width * 2) * (height - 1)) + ((height - 1) - 1);

	indices.emplace_back(nIndices);

	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if (not (z & 1))
		{
			for (int x = 0; x < nWidth; x++)
			{
				if ((x == 0) && (z > 0)) indices[0][j++] = (UINT)(x + (z * nWidth));
				indices[0][j++] = (UINT)(x + (z * nWidth));
				indices[0][j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			for (int x = nWidth - 1; x >= 0; x--)
			{
				if (x == (nWidth - 1)) indices[0][j++] = (UINT)(x + (z * nWidth));
				indices[0][j++] = (UINT)(x + (z * nWidth));
				indices[0][j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}
	BoundingBox::CreateFromPoints(aabb, vertices.size(), (XMFLOAT3*)vertices.data(), sizeof(CVertex));
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{

}
