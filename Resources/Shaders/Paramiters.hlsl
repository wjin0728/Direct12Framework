#ifndef PARAM_DEFINE
#define PARAM_DEFINE


#define DIRECTIONAL_LIGHT 5
#define POINT_LIGHT 5
#define SPOT_LIGHT 5
#define MAX_MATERIAL 10

#define TEXTURE_COUNT 10

#define TERRAIN_SPLAT_COUNT 7

#define CASCADE_COUNT_FLAG 3

//#define FOG

static const float a0 = 1.f;
static const float a1 = 0.01f;
static const float a2 = 0.0001f;


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

struct TerrainData
{
    float3 scale;
    int heightMapIdx;
    
    int splatNum;
    TerrainSplat splat[TERRAIN_SPLAT_COUNT];
};

struct DirectionalLight
{
    float4 color;
    float3 strength;
    float padding1;
    float3 direction;
    float padding2;
};


struct PointLight
{
    float4 color;
    float3 strength;
    float range;
    float3 position;
    float padding;
};


struct SpotLight
{
    float4 color;
    float3 strength;
    float range;
    float3 direction;
    float fallOffStart;
    float3 position;
    float fallOffEnd;
    float3 padding;
    float spotPower;
};

struct LightColor
{
    float4 diffuse;
    float4 specular;
};

cbuffer CBPassData : register(b0)
{
    matrix viewProjMat;
    matrix shadowTransform;
    float4 gFogColor;
    float3 camPos; 
    uint shadowMapIdx; 
    float2 renderTargetSize; 
    float2 passPadding2;
    float deltaTime; 
    float totalTime;
    
    float gFogStart;
    float gFogRange;
    
    TerrainData terrainData;
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

TextureCube skyBoxMap : register(t0, space2);
Texture2D diffuseMap[TEXTURE_COUNT] : register(t0);

Texture2D shadowMap : register(t0, space3);

SamplerState pointWrap : register(s0);
SamplerState pointClamp : register(s1);
SamplerState linearWrap : register(s2);
SamplerState linearClamp : register(s3);
SamplerState anisoWrap : register(s4);
SamplerState anisoClamp : register(s5);
SamplerComparisonState shadowSam : register(s6);

#endif