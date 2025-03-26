#ifndef UTIL_DEFINE
#define UTIL_DEFINE

#include"Paramiters.hlsl"

struct VertexPositionInputs
{
    float4 positionWS;
    float4 positionCS;
};

struct VertexNormalInputs
{
    float3 normalWS;
    float3 tangentWS;
    float3 bitangentWS;
};

struct LightingData
{
    float3 positionWS;
    float3 normalWS;
    float3 cameraDirection;
    float shadowFactor;
};

struct SurfaceData
{
    float3 albedo;
    float3 specular;
    float metallic;
    float smoothness;
};

VertexPositionInputs GetVertexPositionInputs(float3 positionOS)
{
    VertexPositionInputs output;
    
    output.positionWS = mul(float4(positionOS, 1.0f), worldMat);
    output.positionCS = mul(output.positionWS, viewProjMat);
    
    return output;
}

VertexNormalInputs GetVertexNormalInputs(float3 normalOS, float3 tangentOS)
{
    VertexNormalInputs output;
    
    output.normalWS = mul(normalOS, (float3x3)invWorldMat);
    output.tangentWS = mul(tangentOS, (float3x3) invWorldMat);
    output.bitangentWS = cross(output.normalWS, output.tangentWS);
    
    return output;
}

float3 UnpackNormalScale(float4 packedNormal, float scale)
{
    float3 normal;
    normal.xy = packedNormal.xy * 2.0 - 1.0; // 0~1 → -1~1
    normal.xy *= scale; // 강도 적용
    normal.z = sqrt(1.0 - saturate(dot(normal.xy, normal.xy))); // Z 재계산
    return normal;
}


float3 GammaDecoding(float3 color)
{
    return pow(color, 2.2f);
}

float4 GammaDecoding(float4 color)
{
    return pow(color, 2.2f);
}

float3 GammaEncoding(float3 color)
{
    return pow(color, 0.4545f);
}


float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 normal, float3 tangent, float3 bitangent)
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    float3x3 TBN = float3x3(normal, tangent, bitangent);

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

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    float x = 1.0 - cosTheta;
    float x2 = x * x;
    return F0 + (1.0 - F0) * x2 * x2 * x;
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (3.14159 * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

float3 ComputeDirectionalLight(DirectionalLight light, LightingData lightingData, SurfaceData surfaceData)
{
    float3 normal = lightingData.normalWS;
    float3 camDir = lightingData.cameraDirection;
    float3 albedo = surfaceData.albedo;
    float3 specular = surfaceData.specular;
    float metallic = surfaceData.metallic;
    float smoothness = surfaceData.smoothness;
    float3 lightDir = -light.direction;
    float3 lightColor = light.color * light.strength * lightingData.shadowFactor;
   
    smoothness = clamp(surfaceData.smoothness, 0.0, 1.0);
    float roughness = 1.0 - smoothness;
    float3 F0 = float3(0.04, 0.04, 0.04); // 비금속 기본값
    F0 = lerp(F0, albedo, metallic); // 금속성 반영
    
    float3 halfV = normalize(camDir + lightDir);
    float viewHalfDot = max(dot(halfV, normal), 0.f);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, camDir), 0.0);
    
    float3 F = FresnelSchlick(max(dot(halfV, camDir), 0.0), F0);
    float NDF = DistributionGGX(normal, halfV, roughness);
    float G = GeometrySmith(normal, camDir, lightDir, roughness);

    // Specular (Cook-Torrance)
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.0001; // 0 나누기 방지
    specular *= (numerator / denominator);

    // Diffuse (에너지 보존)
    float3 kS = F; // Specular 비율
    float3 kD = 1.0 - kS; // Diffuse 비율
    kD *= 1.0 - metallic; // 금속은 Diffuse 없음
    float3 diffuse = albedo / 3.14159;

    // 최종 직접 조명
    return (kD * diffuse + specular) * lightColor * NdotL;
}


float3 ComputePointLight(PointLight light, LightingData lightingData, SurfaceData surfaceData)
{
    float3 normal = lightingData.normalWS;
    float3 position = lightingData.positionWS;
    float3 camDir = lightingData.cameraDirection;
    float3 albedo = surfaceData.albedo;
    float3 specular = surfaceData.specular;
    float metallic = surfaceData.metallic;
    float smoothness = surfaceData.smoothness;
    float3 lightDir = light.position - position;
    float3 lightColor = light.color * light.strength * lightingData.shadowFactor;
    float distance = length(lightDir);
    
    lightDir = normalize(lightDir);
    
    if (distance > light.range)
        return float3(0.f, 0.f, 0.f);
    
 
    smoothness = clamp(surfaceData.smoothness, 0.0, 1.0);
    float roughness = 1.0 - smoothness;
    float3 F0 = float3(0.04, 0.04, 0.04); // 비금속 기본값
    F0 = lerp(F0, albedo, metallic); // 금속성 반영
    
    float3 halfV = normalize(camDir + lightDir);
    float viewHalfDot = max(dot(halfV, normal), 0.f);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, camDir), 0.0);
    
    float3 F = FresnelSchlick(max(dot(halfV, camDir), 0.0), F0);
    float NDF = DistributionGGX(normal, halfV, roughness);
    float G = GeometrySmith(normal, camDir, lightDir, roughness);
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.0001; // 0 나누기 방지
    specular *= (numerator / denominator);
    
    float3 kS = F;
    float3 kD = 1.0 - kS; 
    kD *= 1.0 - metallic; 
    float3 diffuse = albedo / 3.14159;
    float att = 1.0f / (a0 + distance * a1 + distance * distance * a2);
    
    return (kD * diffuse + specular) * lightColor * NdotL * att;
}


float3 ComputeSpotLight(SpotLight light, LightingData lightingData, SurfaceData surfaceData)
{
    float3 normal = lightingData.normalWS;
    float3 position = lightingData.positionWS;
    float3 camDir = lightingData.cameraDirection;
    float3 albedo = surfaceData.albedo;
    float3 specular = surfaceData.specular;
    float metallic = surfaceData.metallic;
    float smoothness = surfaceData.smoothness;
    float3 lightDir = light.position - position;
    float3 lightColor = light.color * light.strength * lightingData.shadowFactor;
    float distance = length(lightDir);
    
    lightDir = normalize(lightDir);
    
    if (distance > light.range)
        return float3(0.f, 0.f, 0.f);
    
    smoothness = clamp(surfaceData.smoothness, 0.0, 1.0);
    float roughness = 1.0 - smoothness;
    
    float3 F0 = float3(0.04, 0.04, 0.04); // 비금속 기본값
    F0 = lerp(F0, albedo, metallic); // 금속성 반영
    
    float3 halfV = normalize(camDir + lightDir);
    float viewHalfDot = max(dot(halfV, normal), 0.f);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, camDir), 0.0);
    
    float3 F = FresnelSchlick(max(dot(halfV, camDir), 0.0), F0);
    float NDF = DistributionGGX(normal, halfV, roughness);
    float G = GeometrySmith(normal, camDir, lightDir, roughness);
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.0001; // 0 나누기 방지
    specular *= (numerator / denominator);
    
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    float3 diffuse = albedo / 3.14159;
    
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
    
    return (kD * diffuse + specular) * lightColor * NdotL * att;
}


float3 CalculatePhongLight(LightingData lightingData, SurfaceData surfaceData)
{   
    float3 shadowFactor = lightingData.shadowFactor;
    
    float3 finalColor = float3(0, 0, 0);
    
    [unroll(DIRECTIONAL_LIGHT)]
    for (uint i = 0; i < lightNum.x; i++)
    {
        finalColor += ComputeDirectionalLight(dirLights[i], lightingData, surfaceData);
    }
    [unroll(POINT_LIGHT)]
    for (i = 0; i < lightNum.y; i++)
    {
        finalColor += ComputePointLight(pointLights[i], lightingData, surfaceData);
    }
    [unroll(SPOT_LIGHT)]
    for (i = 0; i < lightNum.z; i++)
    {
        finalColor += ComputeSpotLight(spotLights[i], lightingData, surfaceData);
    }
    
    float3 ambient = surfaceData.albedo * 0.1;
    
    return finalColor + ambient;
}

#endif