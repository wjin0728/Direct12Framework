#include "stdafx.h"
#include "Terrain.h"

CTerrain::CTerrain()
	: CComponent(COMPONENT_TYPE::TERRAIN)
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

void CTerrain::Render(const std::shared_ptr<class CCamera>& camera)
{
	mTerrainMaterial->BindShader(FORWARD);
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

float CTerrain::GetHeight(float x, float z)
{
	return mTerrainMesh->GetHeight(x, z);
}

