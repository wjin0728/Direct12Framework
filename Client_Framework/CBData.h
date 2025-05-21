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
	Vec4 projectionParams;

	Vec3 camPos = Vec3::Zero;
	UINT shadowMapIdx{};

	Vec2 renderTargetSize{};
	float deltaTime{};
	float totalTime{};

	int gbufferAlbedoIdx{};
	
	int gbufferNormalIdx{};
	int gbufferEmissiveIdx{};
	int gbufferPosIdx{};
	int gbufferDepthIdx{};
	int lightingTargetIdx{};
	int postProcessIdx{};
	int finalTargetIdx{};
	Vec4 finalRenderTargetAlpha;
};

struct CBObjectData
{
	Matrix worldMAt = Matrix::Identity;
	Matrix invWorldMAt = Matrix::Identity;
	Matrix textureMat = Matrix::Identity;
	int idx0{};
	int idx1{};
	int idx2{};
	int idx3{};
};

struct IBObjectData
{
	Matrix worldMAt = Matrix::Identity;
	Matrix invWorldMAt = Matrix::Identity;
	int idx0{};
};

struct CBLightsData
{
	int type;
	Vec3 color;
	float strength;
	float range;
	float spotAngle;
	float innerSpotAngle;
	Vec3 position;
	float padding0;
	Vec3 direction;
	float padding1;
};

struct AllLightData {
	CBLightsData lights[10];
	int lightCount = 0;
	Vec3 padding0{};
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
	Vec4 color = Vec4::One;
	Vec2 size{};
	Vec2 uvOffset;  
	Vec2 uvScale;   
	float type{ 0 };
	int textureIdx = -1;

	float floatData0{ 0 };
	float floatData1{ 0 };
	float floatData2{ 0 };
	float floatData3{ 0 };

	int intData0{ 0 };
	int intData1{ 0 };
	int intData2{ 0 };
	int intData3{ 0 };

	Vec2 pos{};
	Vec2 vec2Data1{};
	Vec2 vec2Data2{};
	Vec2 vec2Data3{};

	Vec3 vec3Data0{};
	Vec3 vec3Data1{};

	Vec4 vec4Data0{};
	Vec4 vec4Data1{};
};