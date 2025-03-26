#pragma once

constexpr auto DIRECTIONAL_LIGHT = 5;
constexpr auto POINT_LIGHT = 5;
constexpr auto SPOT_LIGHT = 5;
constexpr auto TERRAIN_SPLAT_COUNT = 7;


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
	Vec3 strength;
	Vec3 direction;
	Vec3 padding0;
};

struct CBPointLightInfo
{
	Vec3 color;
	Vec3 strength;
	float range;
	Vec3 position;
	Vec2 padding0;
};

struct CBSpotLightInfo
{
	Vec3 color;
	Vec3 strength;
	float range;
	Vec3 direction;
	float fallOffStart;
	Vec3 position;
	float fallOffEnd;
	Vec4 padding;
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
