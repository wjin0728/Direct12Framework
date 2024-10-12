#ifndef PARAM_DEFINE
#define PARAM_DEFINE


#define DIRECTIONAL_LIGHT 10
#define POINT_LIGHT 10
#define SPOT_LIGHT 10
#define MAX_MATERIAL 10

static const float a0 = 1.f;
static const float a1 = 0.01f;
static const float a2 = 0.0001f;


struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
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
    matrix worldMat : packoffset(c0);
    uint materialIdx;
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



#endif