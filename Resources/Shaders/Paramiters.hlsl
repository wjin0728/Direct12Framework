#ifndef PARAM_DEFINE
#define PARAM_DEFINE


#define DIRECTIONAL_LIGHT 5
#define POINT_LIGHT 5
#define SPOT_LIGHT 5

#define TEXTURE_COUNT 300

#define TERRAIN_SPLAT_COUNT 2

#define CASCADE_COUNT_FLAG 3

#define LIGHTING

//#define FOG

#define SKINNED_ANIMATION_BONES 300

static const float a0 = 1.f;
static const float a1 = 0.01f;
static const float a2 = 0.0001f;

struct DirectionalLight
{
    float3 color;
    float padding1;
    float3 strength;
    float padding2;
    float3 direction;
    float padding3;
};
struct PointLight
{
    float3 color;
    float padding1;
    float3 strength;
    float range;
    float3 position;
    float padding2;
};
struct SpotLight
{
    float3 color;
    float padding1;
    float3 strength;
    float range;
    float3 direction;
    float fallOffStart;
    float3 position;
    float fallOffEnd;
    float spotPower;
    float3 padding2;
};
cbuffer CBPassData : register(b0)
{
    matrix viewProjMat;
    matrix viewMat;
    matrix shadowTransform;
    float3 camPos; 
    uint shadowMapIdx; 
    float2 renderTargetSize; 
    float deltaTime; 
    float totalTime;
    uint gbufferAlbedoIdx;
    uint gbufferNormalIdx;
    uint gbufferDepthIdx;
    uint gbufferPosIdx;
    uint gbufferEmissiveIdx;
    uint lightingTargetIdx;
    uint postProcessIdx;
    uint finalTargetIdx;
};
cbuffer CBObjectData : register(b1)
{
    matrix worldMat;
    matrix invWorldMat;
    matrix texMat;
};
cbuffer CBLightsData : register(b2)
{
    DirectionalLight dirLights[DIRECTIONAL_LIGHT];
    PointLight pointLights[POINT_LIGHT];
    SpotLight spotLights[SPOT_LIGHT];
    uint3 lightNum;
    uint lightPadding;
};
cbuffer CBBoneTransforms : register(b3)
{
    matrix boneTransforms[SKINNED_ANIMATION_BONES];
};

TextureCube skyBoxMap : register(t0, space2);
Texture2D diffuseMap[TEXTURE_COUNT] : register(t0);

SamplerState pointWrap : register(s0);
SamplerState pointClamp : register(s1);
SamplerState linearWrap : register(s2);
SamplerState linearClamp : register(s3);
SamplerState anisoWrap : register(s4);
SamplerState anisoClamp : register(s5);
SamplerComparisonState shadowSam : register(s6);

#endif