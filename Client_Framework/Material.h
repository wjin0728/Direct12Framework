#pragma once
#include"stdafx.h"
#include"CResource.h"
#include"FrameResource.h"
#include"Shader.h"

struct CommonProperties
{
	Vec3 mainColor{};
	int mainTexIdx = -1;

	int normalTexIdx = -1;
	float smoothness{};
	float metallic{};
	float padding{};
};

struct LitProperties
{
	Vec4 mainColor{};
	int mainTexIdx = -1;

	int normalTexIdx = -1;
	float smoothness{};
	float metallic{};
	int emissiveTexIdx = -1;
	Vec3 emissiveColor{};
};

struct TriplanarProperties
{
	float fallOff{};
	float tilling{};
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
	float offset;
	Vec3 bottomColor;
	float distance;

	Vec3 padding1;
	float falloff;
};

struct UIProperties
{
	Vec3 color{};
	float type{0};
	int textureIdx = -1;
};


class CMaterial : public CResource
{
protected:
	std::unique_ptr<BYTE[]> matData{};
	UINT dataSize{};

	UINT mPoolOffset{};

	std::array<std::shared_ptr<CShader>, PASS_TYPE::STENCIL> mShaders{};

	UINT mDirtyFrames{FRAME_RESOURCE_COUNT};

public:
	std::string mShaderName{};
	CMaterial() = default;
	CMaterial(void* data, UINT dataSize);
	static std::shared_ptr<CMaterial> CreateMaterialFromFile(std::ifstream& inFile);

	void Initialize(void* data, UINT dataSize);
	void SetShader(const std::string& name);
	virtual void Update();
	virtual bool BindShader(PASS_TYPE passType);
	virtual void BindDataToShader();
	virtual void ReleaseUploadBuffer() {}

	std::shared_ptr<CShader> GetShader(PASS_TYPE passType) const
	{
		return mShaders[passType];
	}

protected:
	static int GetTextureIdx(std::ifstream& inFile);
};

struct alignas(16) SplatData
{
	Vec4 data[4];
};

struct alignas(16) TerrainData
{
	Vec3 size = Vec3::One;
	float yOffset{};

	int heightMapIdx = -1;
	int splatNum;
	Vec2 heightMapResolution;

	SplatData splats[TERRAIN_SPLAT_COUNT];

	Vec4 alphaMapIdx[TERRAIN_SPLAT_COUNT];
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