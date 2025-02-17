#ifndef UTIL_DEFINE
#define UTIL_DEFINE

#include"Paramiters.hlsl"


float3 GammaDecoding(float3 color)
{
    return pow(color, 2.2f);
}

float3 GammaEncoding(float3 color)
{
    return pow(color, 0.4545f);
}


float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangent)
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;
    
    float3 N = unitNormalW;
    float3 T = normalize(tangent - dot(tangent, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    return mul(normalT, TBN);
}

float CalcShadowFactor(float4 shadowPosH)
{
    //shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;
    diffuseMap[shadowMapIdx].GetDimensions(0, width, height, numMips);

    // Texel size.
    float dx = 1.0f / (float) width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += diffuseMap[shadowMapIdx].SampleCmpLevelZero(shadowSam,
            shadowPosH.xy + offsets[i], depth).r;
    }
    
    return percentLit / 9.0f;
}

LightColor ComputeDirectionalLight(DirectionalLight light, float3 normal, float3 camDir, Material material)
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
            float viewHalfDot = max(dot(halfV, normal), 0.f);
            float3 fresnelFactor = material.fresnelR0 + (1.0 - material.fresnelR0) * pow(1.0 - viewHalfDot, 5);
            
            specularFactor = ((m + 8.f) * pow(viewHalfDot, m)) / 8.f;
            specularColor = specularFactor * (light.color * material.specular).rgb * fresnelFactor;
        }
    }
    LightColor lightColor;
    
    float4 strength = float4(light.strength, 1.f);
    
    lightColor.diffuse = float4(diffuseColor, 1.f) * strength;
    lightColor.specular = float4(specularColor, 1.f) * strength;
    
    return lightColor;
}


LightColor ComputePointLight(PointLight light, float3 position, float3 normal, float3 camDir, Material material)
{
    float3 lightDir = light.position - position;
    
    float3 diffuseColor = float3(0.f, 0.f, 0.f);
    float3 specularColor = float3(0.f, 0.f, 0.f);
    
    float distance = length(lightDir);
    
    LightColor lightColor;
    lightColor.diffuse = float4(0.f, 0.f, 0.f, 1.f);
    lightColor.specular = float4(0.f, 0.f, 0.f, 1.f);
	
    //거리가 Range 안에 있어야 빛을 받음
    if (distance > light.range)
        return lightColor;
    
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
            float viewHalfDot = dot(halfV, camDir);
            float3 fresnelFactor = material.fresnelR0 + (1.0 - material.fresnelR0) * pow(1.0 - viewHalfDot, 5);
            
            specularFactor = ((m + 8.f) * pow(max(viewHalfDot, 0.f), m)) / 8.f;
            specularColor = specularFactor * (light.color * material.specular).rgb * fresnelFactor;
        }
    }
    
    float att = 1.0f / (a0 + distance * a1 + distance * distance * a2);
    
    diffuseColor *= att;
    specularColor *= att;
    
    float4 strength = float4(light.strength, 1.f);
    
    lightColor.diffuse = float4(diffuseColor, 1.f) * strength;
    lightColor.specular = float4(specularColor, 1.f) * strength;
    
    return lightColor;
}


LightColor ComputeSpotLight(SpotLight light, float3 position, float3 normal, float3 camDir, Material material)
{
    float3 lightDir = light.position - position;
    
    float3 diffuseColor = float3(0.f, 0.f, 0.f);
    float3 specularColor = float3(0.f, 0.f, 0.f);
    
    float distance = length(lightDir);
    
    LightColor lightColor;
    lightColor.diffuse = float4(0.f, 0.f, 0.f, 1.f);
    lightColor.specular = float4(0.f, 0.f, 0.f, 1.f);
	
    //거리가 Range 안에 있어야 빛을 받음
    if (distance > light.range)
        return lightColor;
    
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
            float viewHalfDot = dot(halfV, camDir);
            float3 fresnelFactor = material.fresnelR0 + (1.0 - material.fresnelR0) * pow(1.0 - viewHalfDot, 5);
            
            specularFactor = ((m + 8.f) * pow(max(viewHalfDot, 0.f), m)) / 8.f;
            specularColor = specularFactor * (light.color * material.specular).rgb * fresnelFactor;
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
    
    float att = spotFator / (a0 + distance * a1 + distance * distance * a2);
    
    diffuseColor *= att;
    specularColor *= att;
    
    float4 strength = float4(light.strength, 1.f);
    
    lightColor.diffuse = float4(diffuseColor, 1.f) * strength;
    lightColor.specular = float4(specularColor, 1.f) * strength;
    
    return lightColor;
}


LightColor CalculatePhongLight(float3 position, float3 normal, float3 camDir, Material material, float3 shadowFactor)
{
    LightColor finalColor;
    finalColor.diffuse = float4(0.f, 0.f, 0.f, 1.f);
    finalColor.specular = float4(0.f, 0.f, 0.f, 1.f);
    
    [unroll(DIRECTIONAL_LIGHT)]
    for (uint i = 0; i < lightNum.x; i++)
    {
        LightColor color = ComputeDirectionalLight(dirLights[i], normal, camDir, material);
        finalColor.diffuse += color.diffuse * shadowFactor[0];
        finalColor.specular += color.specular * shadowFactor[0];
    }
    [unroll(POINT_LIGHT)]
    for (i = 0; i < lightNum.y; i++)
    {
        LightColor color = ComputePointLight(pointLights[i], position, normal, camDir, material);
        finalColor.diffuse += color.diffuse;
        finalColor.specular += color.specular;
    }
    [unroll(SPOT_LIGHT)]
    for (i = 0; i < lightNum.z; i++)
    {
        LightColor color = ComputeSpotLight(spotLights[i], position, normal, camDir, material);
        finalColor.diffuse += color.diffuse;
        finalColor.specular += color.specular;
    }
    
    return finalColor;
}

#endif