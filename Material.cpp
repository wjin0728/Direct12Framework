#include "stdafx.h"
#include "Material.h"

CMaterial::CMaterial(const XMFLOAT4& albedo) : albedoColor(albedo)
{
	specularColor = albedo;
	specularColor.w *= RandF(0, 1);
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRoot32BitConstants(0, 4, &albedoColor, 16);
	cmdList->SetGraphicsRoot32BitConstants(0, 4, &specularColor, 20);
	cmdList->SetGraphicsRoot32BitConstants(0, 4, &emissiveColor, 24);
}
