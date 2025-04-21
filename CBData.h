#pragma once

constexpr auto DIRECTIONAL_LIGHT = 5;
constexpr auto POINT_LIGHT = 5;
constexpr auto SPOT_LIGHT = 5;
constexpr auto TERRAIN_SPLAT_COUNT = 2;


struct CBPassData
{
	Matrix viewProjMat = Matrix::Identity;
	Matrix viewMat = Matrix::Identity;
	Matrix shadowTransform = Matrix::Identity;

	Vec3 camPos = Vec3::Zero;
	UINT shadowMapIdx{};

	Vec2 renderTargetSize{};

	float deltaTime{};
	float totalTime{};

	int gbufferAlbedoIdx{};
	int gbufferNormalIdx{};
	int gbufferDepthIdx{};
	int gbufferPosIdx{};
	int gbufferEmissiveIdx{};
	int lightingTargetIdx{};
	int postProcessIdx{};
	int finalTargetIdx{};
};

struct CBObjectData
{
	Matrix worldMAt = Matrix::Identity;
	Matrix invWorldMAt = Matrix::Identity;
	Matrix textureMat = Matrix::Identity;
};


struct CBDirectionalLightInfo
{
	Vec3 color;
	float padding1;
	Vec3 strength;
	float padding2;
	Vec3 direction;
	float padding3;
};

struct CBPointLightInfo
{
	Vec3 color;
	float padding1;
	Vec3 strength;
	float range;
	Vec3 position;
	float padding2;
};

struct CBSpotLightInfo
{
	Vec3 color;
	float padding1;
	Vec3 strength;
	float range;
	Vec3 direction;
	float fallOffStart;
	Vec3 position;
	float fallOffEnd;
	float spotPower;
	Vec3 padding2;
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
