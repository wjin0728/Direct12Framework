#pragma once
#include"stdafx.h"
#include "GameObject.h"

class CHeightMapImage;
class CHeightMapGridMesh;

class CTerrain :
    public CGameObject
{
private:
    std::shared_ptr<CHeightMapImage> heightMap;
    std::vector<std::shared_ptr<CHeightMapGridMesh>> gridMeshes;

    int width;
    int height;
    XMFLOAT3 scale;

public:
    CTerrain() = default;
    CTerrain(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, LPCTSTR fileName, int
        _width, int _length, int _blockWidth, int _blockLength, XMFLOAT3 _scale, XMFLOAT4 color);
    virtual ~CTerrain();

public:
    int GetHeightMapWidth();
    int GetHeightMapLength();
    XMFLOAT3 GetScale() { return scale; }
    float GetWidth() { return(width * scale.x); }
    float GetLength() { return(height * scale.z); }

    virtual void Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera);
};

