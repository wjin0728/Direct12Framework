#include "stdafx.h"
#include "Terrain.h"
#include"HeightMapImage.h"
#include"GridMesh.h"

CTerrain::CTerrain(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, LPCTSTR fileName,
	int _width, int _length, int _blockWidth, int _blockLength, XMFLOAT3 _scale, XMFLOAT4 color)
	: width(_width), height(_length), scale(_scale)
{
	int cxQuadsPerBlock = _blockWidth - 1;
	int czQuadsPerBlock = _blockLength - 1;

	heightMap = std::make_shared<CHeightMap>(fileName, width, height, scale);

	long cxBlocks = (width - 1) / cxQuadsPerBlock;
	long czBlocks = (height - 1) / czQuadsPerBlock;

	gridMeshes.resize(cxBlocks * czBlocks);

	for (int z = 0, zStart = 0; z < czBlocks; z++) {
		for (int x = 0, xStart = 0; x < cxBlocks; x++) {
			xStart = x * cxQuadsPerBlock;
			zStart = z * czQuadsPerBlock;

			gridMeshes[x + (z * cxBlocks)] = std::make_shared<CHeightMapGridMesh>(xStart,
				zStart, _blockWidth, _blockLength, heightMap.get());
		}
	}


	AddMaterial(color);
	materials[0]->specularColor.w = 0.01;
}

CTerrain::~CTerrain()
{
}

int CTerrain::GetHeightMapWidth()
{
	return heightMap->GetHeightMapWidth();
}

int CTerrain::GetHeightMapLength()
{
	return heightMap->GetHeightMapLength();
}

void CTerrain::Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera)
{
	materials[0]->UpdateShaderVariables(cmdList);
	for (auto& mesh : gridMeshes) {
		if (pCamera->IsInFrustum(mesh->aabb)) {
			mesh->Render(cmdList, 0);
		}
	}
}
