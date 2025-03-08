#pragma once
#include"stdafx.h"
#include"CResource.h"



class CMaterial : public CResource
{
public:
	Color mAlbedoColor = Color(1.f,1.f,1.f,1.f);
	Color mSpecularColor = Color(1.f, 1.f, 1.f, 1.f);
	Color mEmissiveColor = Color(0.f, 0.f, 0.f, 1.f);
	Vec3 mFresnelR0 = Vec3(0.04f, 0.04f, 0.04f);
	UINT mPoolOffset{};

	int mDiffuseMapIdx = -1;
	int mNormalMapIdx = -1;

	std::shared_ptr<class CShader> mShader{};

public:
	CMaterial();
	virtual void Update();
	virtual void BindToShader();
	virtual void ReleaseUploadBuffer() {}
	static std::shared_ptr<CMaterial> CreateMaterialFromFile(std::ifstream& inFile);
};

class CTerrainMaterial : public CMaterial
{
public:
	int mDetailMap1Idx = -1;

public:
	CTerrainMaterial() = default;
	virtual void Update();

};