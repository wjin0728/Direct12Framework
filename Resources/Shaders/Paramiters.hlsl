#ifndef PARAM_DEFINE
#define PARAM_DEFINE

#define TEXTURE_COUNT 300
#define TERRAIN_SPLAT_COUNT 2
#define CASCADE_COUNT_FLAG 3
#define MAX_VERTEX_INFLUENCES 4
#define LIGHTING

//#define FOG

#define SKINNED_ANIMATION_BONES 300

static const float a0 = 1.f;
static const float a1 = 0.01f;
static const float a2 = 0.0001f;

cbuffer CBPassData : register(b0)
{
    matrix viewProjMat;
    matrix shadowTransform;
    matrix viewMat;
    matrix UIProjMat;
    float3 camPos; 
    uint shadowMapIdx; 
    float2 renderTargetSize; 
    float deltaTime; 
    float totalTime;
    int gbufferAlbedoIdx;
    //normal + metallic
    int gbufferNormalIdx;
    //depth
    int gbufferEmissiveIdx;
    int gbufferPosIdx;
    int gbufferDepthIdx;
    //world position + smoothness
    //emissive + shadow
    int lightingTargetIdx;
    int postProcessIdx;
    int finalTargetIdx;
};
cbuffer CBObjectData : register(b1)
{
    matrix worldMat;
    matrix invWorldMat;
    matrix texMat;
    int idx0;
    int idx1;
    int idx2;
    int idx3;
};
struct CBLightsData
{
    int lightType;
    float3 lColor;
    float strength;
    float range;
    float spotAngle;
    float innerSpotAngle;
    float3 positionWS;
    float padding0;
    float3 directionWS;
    float padding1;
};

cbuffer AllLightData : register(b2)
{
    CBLightsData lights[10];
    int lightCount = 0;
    float3 lightPadding;
};

cbuffer CBBoneTransforms : register(b3)
{
    matrix boneTransforms[SKINNED_ANIMATION_BONES];
};

struct CBUIData
{
    float3 color;
    float depth;
    float2 size;
    float2 uvOffset;
    float2 uvScale;
    float type;
    int textureIdx;

    float floatData0;
    float floatData1;
    float floatData2;
    float floatData3;

    int intData0;
    int intData1;
    int intData2;
    int intData3;

    float2 pos;
    float2 vec2Data1;
    float2 vec2Data2;
    float2 vec2Data3;

    float3 vec3Data0;
    float3 vec3Data1;

    float4 vec4Data0;
    float4 vec4Data1;
};

TextureCube skyBoxMap : register(t0, space2);
Texture2D diffuseMap[TEXTURE_COUNT] : register(t0);
StructuredBuffer<CBUIData> UIData : register(t0, space3);

SamplerState pointWrap : register(s0);
SamplerState pointClamp : register(s1);
SamplerState linearWrap : register(s2);
SamplerState linearClamp : register(s3);
SamplerState anisoWrap : register(s4);
SamplerState anisoClamp : register(s5);
SamplerComparisonState shadowSam : register(s6);

#endif