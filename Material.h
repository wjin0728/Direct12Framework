#pragma once
#include"stdafx.h"

class CMaterial
{
public:
	XMFLOAT4 albedoColor{};
	XMFLOAT4 specularColor{};
	XMFLOAT4 emissiveColor{};
	XMFLOAT3 fresnelR0{};

	int diffuseMapIdx = -1;
	int normalMapIdx = -1;

	int dirtyFramesNum = FRAMERESOURCE_NUM;

public:
	CMaterial() = default;
	CMaterial(const XMFLOAT4& albedo);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* cmdList);
};

