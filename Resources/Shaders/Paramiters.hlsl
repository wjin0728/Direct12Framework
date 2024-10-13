#ifndef PARAM_DEFINE
#define PARAM_DEFINE


#define DIRECTIONAL_LIGHT 10
#define POINT_LIGHT 10
#define SPOT_LIGHT 10
#define MAX_MATERIAL 10

#define TEXTURE_COUNT 10

static const float a0 = 1.f;
static const float a1 = 0.01f;
static const float a2 = 0.0001f;


struct Material
{
    float4 albedo;
    float4 specular;
    float4 emissive;
    float3 fresnelR0;
    int diffuseMapIdx;
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
    matrix viewMat;
    matrix projMat;
    matrix viewProjMat;
    float3 camPos;
    float passPadding;
    float2 renderTargetSize;
    float deltaTime;
    float totalTime;
};

cbuffer CBObjectData : register(b1)
{
    matrix worldMat;
    int materialIdx;
    float3 objectPadding;
};

cbuffer CBLightsData : register(b2)
{
    DirectionalLight dirLights[DIRECTIONAL_LIGHT];
    PointLight pointLights[POINT_LIGHT];
    SpotLight spotLights[SPOT_LIGHT];
    uint3 lightNum;
    uint lightPadding;
};


Texture2D diffuseMap[TEXTURE_COUNT] : register(t0);

StructuredBuffer<Material> materials : register(t0, space1);

SamplerState pointWrap : register(s0);
SamplerState pointClamp : register(s1);
SamplerState linearWrap : register(s2);
SamplerState linearClamp : register(s3);
SamplerState anisoWrap : register(s4);
SamplerState anisoClamp : register(s5);

#endif