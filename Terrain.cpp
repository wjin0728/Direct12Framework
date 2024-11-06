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
	mQuadTree->Render(camera);
}

void CTerrain::SetHeightMapGridMesh(const std::shared_ptr<CHeightMapGridMesh>& mesh)
{
	if (mesh == mTerrainMesh) {
		return;
	}
	mQuadTree.reset();
	mTerrainMesh = mesh;

	mQuadTree = std::make_shared<CQuadTree>();
	mQuadTree->Initialize(mTerrainMesh);
}

void CTerrain::SetMaterial(const std::shared_ptr<CTerrainMaterial>& material)
{
	mTerrainMaterial = material;
}

