#pragma once
#include"stdafx.h"

class CMaterial
{
public:
	XMFLOAT4 albedoColor = XMFLOAT4(1.f,1.f,1.f,1.f);
	XMFLOAT4 specularColor = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	XMFLOAT4 emissiveColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	XMFLOAT3 fresnelR0 = XMFLOAT3(0.04, 0.04, 0.04);

	int diffuseMapIdx = -1;
	int normalMapIdx = -1;

	int dirtyFramesNum = FRAMERESOURCE_NUM;

public:
	CMaterial() = default;
	CMaterial(const XMFLOAT4& albedo);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* cmdList);
};

