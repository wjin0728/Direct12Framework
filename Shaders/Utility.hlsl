#ifndef UTIL_DEFINE
#define UTIL_DEFINE

#include"Paramiters.hlsl"

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
            
            specularFactor = ((m + 8.f) * pow(max(dot(halfV, normal), 0.f), m)) / 8.f;
            
            specularColor = specularFactor * (light.color * material.specular).rgb;
        }
    }
    
    LightColor color = (LightColor) 0.f;
    
    float4 strength = float4(light.strength, 1.f);
    
    color.diffuse = float4(diffuseColor, 1.f) * strength;
    color.specular = float4(specularColor, 1.f) * strength;
    
    return color;
}


LightColor ComputePointLight(PointLight light, float3 position, float3 normal, float3 camDir, Material material)
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
    
    LightColor color = (LightColor) 0.f;
    
    float4 strength = float4(light.strength, 1.f);
    
    color.diffuse = float4(diffuseColor, 1.f) * strength;
    color.specular = float4(specularColor, 1.f) * strength;
    
    return color;
}


LightColor ComputeSpotLight(SpotLight light, float3 position, float3 normal, float3 camDir, Material material)
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
    
    float att = spotFator / (a0 + distance * a1 + distance * distance * a2);
    
    diffuseColor *= att;
    specularColor *= att;
    
    LightColor color = (LightColor) 0.f;
    
    float4 strength = float4(light.strength, 1.f);
    
    color.diffuse = float4(diffuseColor, 1.f) * strength;
    color.specular = float4(specularColor, 1.f) * strength;
    
    return color;
}


LightColor CalculatePhongLight(float3 position, float3 normal, float3 camDir, Material material)
{
    LightColor finalColor = (LightColor) 0.f;
    
    [unroll(DIRECTIONAL_LIGHT)]
    for (uint i = 0; i < lightNum.x; i++)
    {
        LightColor color = ComputeDirectionalLight(dirLights[i], normal, camDir, material);
        finalColor.diffuse += color.diffuse;
        finalColor.specular += color.specular;
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