#pragma once

constexpr auto DIRECTIONAL_LIGHT = 5;
constexpr auto POINT_LIGHT = 5;
constexpr auto SPOT_LIGHT = 5;
constexpr auto TERRAIN_SPLAT_COUNT = 2;

struct CBPassData
{
	Matrix viewProjMat = Matrix::Identity;
	Matrix shadowTransform = Matrix::Identity;
	Matrix viewMat = Matrix::Identity;
	Matrix uiTransform = Matrix::Identity;

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

struct CBLightsData
{
	int type;
	Vec3 color;
	float strength;
	float range;
	float spotAngle;
	float innerSpotAngle;
	Matrix worldMat = Matrix::Identity;

};

struct BillboardData
{
	Vec3 position{};
	Vec2 size{};
	int materialIdx = -1;
	Matrix textureMat = Matrix::Identity;
};

struct CBUIData
{
	Vec3 color{};
	float padding{};
	Vec2 size{};
	Vec2 position{};
	Vec2 scale{};
	Vec2 uvOffset;  
	Vec2 uvScale;   
	float type{ 0 };
	int textureIdx = -1;
};