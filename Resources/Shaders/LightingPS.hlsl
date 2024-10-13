#define DIRECTIONAL_LIGHT 10
#define POINT_LIGHT 10
#define SPOT_LIGHT 10
#define MAX_MATERIAL 10


static const float a0 = 1.f;
static const float a1 = 0.01f;
static const float a2 = 0.0001f;


struct Material
{
    float4 albedo;
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



cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
    Material material : packoffset(c4);
};

cbuffer CBPassData : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 camPos : packoffset(c8);
};


cbuffer CBLightsData : register(b2)
{
    DirectionalLight dirLights[DIRECTIONAL_LIGHT];
    PointLight pointLights[POINT_LIGHT];
    SpotLight spotLights[SPOT_LIGHT];
    uint3 lightNum;
};


float3 ComputeDirectionalLight(DirectionalLight light, float3 normal, float3 camDir, Material material)
{
    float3 lightDir = -light.direction;
    
    float3 diffuseColor = float3(0.f, 0.f, 0.f);
    float3 specularColor = float3(0.f, 0.f, 0.f);
    
    
    //디퓨즈, 스페큘러항 계산
    float diffuseFactor = dot(lightDir, normal);
    float specularFactor = 0.f;
    
    if (diffuseFactor > 0.0f)
    {
        diffuseColor = diffuseFactor * (light.color * material.albedo).rgb;
        
        float shininess = material.specular.a;
        
        if (shininess != 0.0f)
        {
            float m = shininess * 256.f;
            float3 halfV = normalize(camDir + lightDir);
            
            specularFactor = ((m + 8.f) * pow(max(dot(halfV, normal), 0.f), m)) / 8.f;
            
            specularColor = specularFactor * (light.color * material.specular).rgb;
        }
    }
    
    return light.strength * (diffuseColor + specularColor);
}

float3 ComputePointLight(PointLight light, float3 position, float3 normal, float3 camDir, Material material)
{
    float3 lightDir = light.position - position;
    
    float3 diffuseColor = float3(0.f, 0.f, 0.f);
    float3 specularColor = float3(0.f, 0.f, 0.f);
    
    float distance = length(lightDir);
	
    //거리가 Range 안에 있어야 빛을 받음
    if (distance > light.range)
        return float3(0.f, 0.f, 0.f);
    
    lightDir = normalize(lightDir);
 
    
    
    //디퓨즈, 스페큘러항 계산
    float diffuseFactor = dot(lightDir, normal);
    float specularFactor = 0.f;
    
    if (diffuseFactor > 0.0f)
    {
        diffuseColor = diffuseFactor * (light.color * material.albedo).rgb;
        
        float shininess = material.specular.a;
        
        if (shininess != 0.0f)
        {
            float m = shininess * 256.f;
            float3 halfV = normalize(camDir + lightDir);
            
            specularFactor = ((m + 8.f) * pow(max(dot(halfV, normal), 0.f), m)) / 8.f;
            
            specularColor = specularFactor * (light.color * material.specular).rgb;
        }
    }
    
    float att = 1.0f / (a0 + distance * a1 + distance * distance * a2);
    
    diffuseColor *= att;
    specularColor *= att;
    
    return light.strength * (diffuseColor + specularColor);
}


float3 ComputeSpotLight(SpotLight light, float3 position, float3 normal, float3 camDir, Material material)
{
    float3 lightDir = light.position - position;
    
    float3 diffuseColor = float3(0.f, 0.f, 0.f);
    float3 specularColor = float3(0.f, 0.f, 0.f);
    
    float distance = length(lightDir);
	
    //거리가 Range 안에 있어야 빛을 받음
    if (distance > light.range)
        return float3(0.f, 0.f, 0.f);
    
    lightDir = normalize(lightDir);
 
    
    
    //디퓨즈, 스페큘러항 계산
    float diffuseFactor = dot(lightDir, normal);
    float specularFactor = 0.f;
    
    if (diffuseFactor > 0.0f)
    {
        diffuseColor = diffuseFactor * (light.color * material.albedo).rgb;
        
        float shininess = material.specular.a;
        
        if (shininess != 0.0f)
        {
            float m = shininess * 256.f;
            float3 halfV = normalize(camDir + lightDir);
            
            specularFactor = ((m + 8.f) * pow(max(dot(halfV, normal), 0.f), m)) / 8.f;
            
            specularColor = specularFactor * (light.color * material.specular).rgb;
        }
    }
    
    float spotFator = 0.f;
    float fRho = dot(-lightDir, light.direction);
    
    if (fRho > light.fallOffStart)
        spotFator = 1.f;
    else if (fRho <= light.fallOffEnd)
        spotFator = 0.f;
    else
    {
        spotFator = pow((light.fallOffEnd - fRho) / (light.fallOffEnd - light.fallOffStart), light.spotPower);
    }
    
    float att = spotFator / (a0 + distance*a1 + distance * distance * a2);
    
    diffuseColor *= att;
    specularColor *= att;
    
    return light.strength * (diffuseColor + specularColor);
}



struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float3 worldNormal : NORMAL;
};






float4 PS_Main(VS_OUTPUT input) : SV_TARGET
{ 
    float3 normal = normalize(input.worldNormal);
    float3 camDir = normalize(camPos - input.worldPos.xyz);
    
    float3 finalColor = 0.1f * material.albedo;
    
    [unroll(DIRECTIONAL_LIGHT)]
    for (uint i = 0; i < lightNum.x; i++)
    {
        finalColor += ComputeDirectionalLight(dirLights[i], normal, camDir, material);

    }
    [unroll(POINT_LIGHT)]
    for (i = 0; i < lightNum.y; i++)
    {
        finalColor += ComputePointLight(pointLights[i], input.worldPos.xyz, normal, camDir, material);
    }
    [unroll(SPOT_LIGHT)]
    for (i = 0; i < lightNum.z; i++)
    {
        finalColor += ComputeSpotLight(spotLights[i], input.worldPos.xyz, normal, camDir, material);
    }
    
    
    return float4(finalColor, 1.f);
}