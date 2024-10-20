#include "stdafx.h"
#include "GridMesh.h"
#include"HeightMapImage.h"


Vec3 CHeightMapGridMesh::CalculateNormal(int x, int z, CHeightMap* heightMapImage)
{
	Vec3 normal = { 0.f,0.f,0.f };

	std::queue<Vec3> normals;

	if ((x < (width - 1)) && (z < (height - 1))) 
		normals.push(heightMapImage->GetHeightMapNormal(x, z));
	if ((z > 0) && (x < (width - 1))) 
		normals.push(heightMapImage->GetHeightMapNormal(x, z - 1));

	Vec3 scale = heightMapImage->GetScale();

	const BYTE* heightMapPixels = heightMapImage->GetHeightMapPixels();

	if (z & 1) 
	{
		if ((x > 0) && (z < (height - 1)))
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x - 1 + width * z] * scale.y;
			float y3 = (float)heightMapPixels[x + width * (z + 1)] * scale.y;

			Vec3 edge1 = { -scale.x, y2 - y1, 0.f };
			Vec3 edge2 = { 0.f, y3 - y1, scale.z };

			normals.push(edge1.Cross(edge2));
		}
		if ((x > 0) && (z > 0))
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x + width * (z - 1)] * scale.y;
			float y3 = (float)heightMapPixels[x - 1 + width * z] * scale.y;

			Vec3 edge1 = { 0.f, y2 - y1, -scale.z };
			Vec3 edge2 = { -scale.x, y3 - y1, 0.f };

			normals.push(edge1.Cross(edge2));
		}
		if ((x < (width - 1)) && (z < (height - 1)))
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x + 1 + width * (z + 1)] * scale.y;
			float y3 = (float)heightMapPixels[x + 1 + width * z] * scale.y;

			Vec3 edge1 = { scale.x, y2 - y1, scale.z };
			Vec3 edge2 = { scale.x, y3 - y1, 0 };

			normals.push(edge1.Cross(edge2));
		}
		if ((x < (width - 1)) && (z > 0))
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x + 1 + width * z] * scale.y;
			float y3 = (float)heightMapPixels[x + 1 + width * (z - 1)] * scale.y;

			Vec3 edge1 = { scale.x, y2 - y1, 0.f };
			Vec3 edge2 = { scale.x, y3 - y1, -scale.z };

			normals.push(edge1.Cross(edge2));
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

			Vec3 edge1 = { -scale.x, y2 - y1, scale.z };
			Vec3 edge2 = { 0.f, y3 - y1, scale.z };

			normals.push(edge1.Cross(edge2));
		}
		if ((x > 0) && (z > 0))
		{
			float y1 = (float)heightMapPixels[x + width * z] * scale.y;
			float y2 = (float)heightMapPixels[x + width * (z - 1)] * scale.y;
			float y3 = (float)heightMapPixels[x - 1 + width * (z - 1)] * scale.y;

			Vec3 edge1 = { -scale.x, y2 - y1, -scale.z };
			Vec3 edge2 = { 0.f, y3 - y1, -scale.z };

			normals.push(edge1.Cross(edge2));
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

	Vec3 scale = heightMapImage->GetScale();
	const BYTE* heightMapPixels = heightMapImage->GetHeightMapPixels();

	for (int z = zStart; z < maxZ; z++) {
		for (int x = xStart; x < maxX; x++) {
			int y = heightMapPixels[x + (z * nWidth)];

			Vec3 position = Vec3( x - maxX/2, y, z - maxZ/2) * scale;
			Vec3 normal = CalculateNormal(x, z, heightMapImage);
			vertices.emplace_back(position, normal);
		}
	}
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{

}
