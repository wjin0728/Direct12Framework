#include "stdafx.h"
#include "Terrain.h"

CTerrain::CTerrain()
{
}

CTerrain::~CTerrain()
{
}

void CTerrain::Awake()
{
}

void CTerrain::Start()
{
}

void CTerrain::Update()
{
}

void CTerrain::LateUpdate()
{
}

void CTerrain::Render(const std::shared_ptr<class CCamera>& camera, int pass)
{
	mTerrainMaterial->BindShader((PASS_TYPE)pass);
	mTerrainMaterial->BindDataToShader();
	mQuadTree->Render(camera);
}

void CTerrain::SetHeightMapGridMesh(const std::shared_ptr<CHeightMapGridMesh>& mesh)
{
	if (mesh == mTerrainMesh) {
		return;
	}
	mQuadTree.reset();
	mTerrainMesh = mesh;
	mScale = mesh->GetScale();

	mQuadTree = std::make_shared<CQuadTree>();
	mQuadTree->Initialize(mTerrainMesh);
}

void CTerrain::SetMaterial(const std::shared_ptr<CTerrainMaterial>& material)
{
	mTerrainMaterial = material;

	if (mTerrainMesh) {
		material->data.heightMapIdx = mTerrainMesh->GetHeightMapIdx();
		material->data.yOffset = mTerrainMesh->GetOffset().y;
		material->data.heightMapResolution = { (float)mTerrainMesh->GetWidth(), (float)mTerrainMesh->GetWidth() };
	}
}

void CTerrain::MakeNavMap(const std::string& fileName, UINT resolution)
{
	mNavMapResolution = resolution;
	std::vector<BYTE> data;
	std::ifstream file("..\\Resources\\Textures\\" + fileName + ".raw", std::ios::binary);
	if (!file.is_open()) {
		return;
	}
	UINT navMapSize = resolution * resolution;
	data.resize(navMapSize);
	file.read(reinterpret_cast<char*>(data.data()), navMapSize * sizeof(BYTE));

	mNavMapData.resize(navMapSize);
	for (UINT y = 0; y < resolution; y++)
	{
		for (UINT x = 0; x < resolution; x++)
		{
			mNavMapData[x + ((resolution - 1 - y) * resolution)] = data[x + (y * resolution)];
		}
	}
}

float CTerrain::GetHeight(float x, float z)
{
	return mTerrainMesh->GetHeight(x, z);
}

bool CTerrain::CanMove(float x, float z)
{
	if (mNavMapData.empty()) {
		return false;
	}
	float localX = x - mTerrainMesh->GetOffset().x;
	float localZ = z - mTerrainMesh->GetOffset().z;
	if (localX < 0.0f || localZ < 0.0f || localX >= mScale.x || localZ >= mScale.z) {
		return false;
	}
	float xIndex = localX / (mScale.x / (mNavMapResolution));
	float zIndex = localZ / (mScale.z / (mNavMapResolution));
	zIndex = mNavMapResolution - zIndex;
	xIndex = static_cast<int>(xIndex);
	zIndex = static_cast<int>(zIndex);

	int idx = xIndex + (zIndex * mNavMapResolution);
	if (mNavMapData[idx] != 0) {
		return true;
	}

	return false;
}



bool CTerrain::Raycast(const Ray& ray, float maxDist, float& distance)
{
	float step = 0.1f;

	for (float d = 0.f; d < maxDist; d += step) {
		Vec3 pos = ray.position + ray.direction * d;
		float terrainY = GetHeight(pos.x, pos.z);
		if (pos.y < terrainY) {
			distance = d;
			return true;
		}
	}
	return false;
}

