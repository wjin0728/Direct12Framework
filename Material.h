#pragma once
#include"stdafx.h"
#include"CResource.h"
#include"FrameResource.h"
#include"Shader.h"


struct CommonProperties
{
	int mainTexIdx = -1;
	Vec3 mainColor{};

	int normalTexIdx = -1;
	float smoothness{};
	float metallic{};
	float padding{};
};

struct TriplanarProperties
{
	float fallOff{};
	Vec2 tilling{};
	float padding{};

	int topTexIdx = -1;
	int topNormalIdx = -1;
	int sideTexIdx = -1;
	int sideNormalIdx = -1;
};

struct VegitationProperties
{
	Vec3 leafColor{};
	float leafSmoothness{};
	float leafMetallic{};
	int leafTexIdx = -1;
	int leafNormalIdx = -1;
	float padding0{};

	Vec3 trunkColor{};
	float trunkSmoothness{};
	float trunkMetallic{};
	int trunkTexIdx = -1;
	int trunkNormalIdx = -1;
	float padding1{};
};

struct SkyboxProperties
{
	Vec3 topColor;
	Vec3 bottomColor;

	float offset;
	float distance;
	float falloff;

	Vec3 padding;
};


class CMaterial : public CResource
{
protected:
	std::unique_ptr<BYTE[]> matData{};
	UINT dataSize{};

	UINT mPoolOffset{};

	std::array<std::shared_ptr<CShader>, PASS_TYPE::PASS_TYPE_COUNT> mShaders{};

	UINT mDirtyFrames{FRAME_RESOURCE_COUNT};

public:
	CMaterial() = default;
	CMaterial(void* data, UINT dataSize);
	static std::shared_ptr<CMaterial> CreateMaterialFromFile(std::ifstream& inFile);

	void Initialize(void* data, UINT dataSize);
	void SetShader(const std::string& name);
	virtual void Update();
	virtual bool BindShader(PASS_TYPE passType);
	virtual void BindDataToShader();
	virtual void ReleaseUploadBuffer() {}

protected:
	static int GetTextureIdx(std::ifstream& inFile);
};

struct TerrainData
{
	Vec3 size = Vec3::One;
	int heightMapIdx = -1;

	int splatNum;
	int alphaMapIdx[TERRAIN_SPLAT_COUNT];
	int diffuseIdx[TERRAIN_SPLAT_COUNT * 4];
	int normalIdx[TERRAIN_SPLAT_COUNT * 4];

	float metallic[TERRAIN_SPLAT_COUNT * 4];
	float smoothness[TERRAIN_SPLAT_COUNT * 4];
};

class CTerrainMaterial : public CMaterial
{
public:
	TerrainData data;

public:
	CTerrainMaterial() = default;
	virtual void Update();

	void LoadTerrainData(std::ifstream& inFile);
	Vec3 GetSize() const { return data.size; }

public:

};