#pragma once

constexpr auto DIRECTIONAL_LIGHT = 5;
constexpr auto POINT_LIGHT = 5;
constexpr auto SPOT_LIGHT = 5;
constexpr auto TERRAIN_SPLAT_COUNT = 7;

struct CBMaterialDate {
	Color albedoColor{};
	Color specularColor{};
	Color emissiveColor{};
	Vec3 fresnelR0{};
	int diffuseMapIdx = -1;
	int normalMapIdx = -1;
	Vec3 padding1{};
};

struct TerrainSplat
{
	int layerNum;
	struct Layer
	{
		int diffuseIdx;
		int normalIdx;

		float metallic;
		float smoothness;
	} layer[4];
};

struct CBTerrainDate {
	Vec3 scale = Vec3::One;
	int heightMapIdx = -1;

	int splatNum;
	TerrainSplat splats[TERRAIN_SPLAT_COUNT];
};

struct CBPassData
{
	Matrix viewProjMat = Matrix::Identity;
	Matrix shadowTransform = Matrix::Identity;

	Vec4 gFogColor = { 0.7f, 0.7f, 0.7f, 1.0f };

	Vec3 camPos = Vec3::Zero;
	UINT shadowMapIdx{};

	Vec2 renderTargetSize{};
	Vec2 padding2;

	float deltaTime{};
	float totalTime{};
	float gFogStart = 50.0f;
	float gFogRange = 300.0f;

	CBTerrainDate terrainMat;
};

struct CBObjectData
{
	Matrix worldMAt = Matrix::Identity;
	Matrix invWorldMAt = Matrix::Identity;
	Matrix textureMat = Matrix::Identity;
};

struct CBDirectionalLightInfo
{
	Color color;
	Vec3 strength;
	float padding1;
	Vec3 direction;
	float padding2;
};

struct CBPointLightInfo
{
	Color color;
	Vec3 strength;
	float range;
	Vec3 position;
	float padding;
};

struct CBSpotLightInfo
{
	Color color;
	Vec3 strength;
	float range;
	Vec3 direction;
	float fallOffStart;
	Vec3 position;
	float fallOffEnd;
	Vec3 padding;
	float spotPower;
};


struct CBLightsData
{
	CBDirectionalLightInfo dirLights[DIRECTIONAL_LIGHT];
	CBPointLightInfo pointLights[POINT_LIGHT];
	CBSpotLightInfo spotLights[SPOT_LIGHT];
	XMUINT3 lightNum;
	UINT padding;
};

struct BillboardData
{
	Vec3 position{};
	Vec2 size{};
	int materialIdx = -1;
	Matrix textureMat = Matrix::Identity;
};
