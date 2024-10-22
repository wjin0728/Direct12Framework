#include "stdafx.h"
#include "Terrain.h"

CTerrain::CTerrain(const std::shared_ptr<CHeightMapGridMesh>& mesh, const std::shared_ptr<CTerrainMaterial>& material)
	: CComponent(COMPONENT_TYPE::TERRAIN)
{
	mTerrainMesh = mesh;
	mTerrainMaterial = material;
}

CTerrain::~CTerrain()
{
}

void CTerrain::Awake()
{
	mQuadTree = std::make_shared<CQuadTree>();
	mQuadTree->Initialize(mTerrainMesh);
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

void CTerrain::FixedUpdate()
{
}

void CTerrain::Render(const std::shared_ptr<class CCamera>& camera)
{
	mQuadTree->Render(camera);
}

