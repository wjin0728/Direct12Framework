#pragma once

constexpr auto DIRECTIONAL_LIGHT = 5;
constexpr auto POINT_LIGHT = 5;
constexpr auto SPOT_LIGHT = 5;
constexpr auto TERRAIN_SPLAT_COUNT = 2;
constexpr auto CASCADE_COUNT = 4;

struct CBPassData
{
	Matrix viewProjMat = Matrix::Identity;
	Matrix viewMat = Matrix::Identity;
	Matrix invViewMat = Matrix::Identity;
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

struct CBAllShadowData
{
	Matrix shadowViewMat;
	Vec3 lightDirection = Vec3::Zero;
	float shadowMapSize = 1024.0f; 
	int cascadeCount = CASCADE_COUNT; 
	float shadowPartition = 0.1f; 
	float cascadeBlend = 0.1f; 
	float texelSize = 1.0f / shadowMapSize; 
	float nativeTexelSize = 1.0f / shadowMapSize;
	int shadowMapIdx{-1};
	
	float minBorder;
	float maxBorder;
	Vec4 cascadeOffset[CASCADE_COUNT]; 
	Vec4 cascadeScale[CASCADE_COUNT]; 
	float cascadeFrustumEyeSpaceDepth[CASCADE_COUNT];
};

struct CBObjectData
{
	Matrix worldMAt = Matrix::Identity;
	Matrix invWorldMAt = Matrix::Identity;
	Matrix textureMat = Matrix::Identity;
	float hitFactor = 0.0f;
	float alpha = 1.0f;
	Vec2 padding0{};
	int idx0{};
	int idx1{};
	int idx2{};
	int idx3{};
	int health;
	int maxHealth;
	Vec2 padding1{};
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
	Vec2 scale = Vec2::One; // Scale for the UI element
	Vec2 vec2Data1{};
	Vec2 vec2Data2{};

	Vec3 vec3Data0{};
	Vec3 vec3Data1{};

	Vec4 vec4Data0{};
	Vec4 vec4Data1{};
};

struct ParticleSpawnData
{
	float ageRate;
	float rotationSpeed;
	float startRotation;
	float startSize;
	float speed;
	Vec3 direction;
	Vec3 startLocation; 
	float random;
	Color startColor;
};

struct ParticleMotion
{
	Vec3 Position;
	Vec3 Velocity;
	float Age;
	uint32_t ResetDataIndex;
};

struct CBParticleData
{
	Color color = Vec4::One; // RGBA color
	Vec3 position = Vec3::Zero;
	float age = 0.0f; // Age of the particle
	Vec3 velocity = Vec3::Zero;
	float size = 1.0f;
	float rotation = 0.0f;
	int materialIdx = -1; // Index of the material used for the particle
	float padding0{}; // Padding to align the structure
};