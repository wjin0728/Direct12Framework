// Utility.hlsl

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
    float3 lightPos;
    float3 positionWS;
    float3 normalWS;
    float3 cameraDirection;
    float shadowFactor;
};

struct SurfaceData
{
    float3 albedo;
    float3 specular;
    float3 emissive;
    float metallic;
    float smoothness;
};

inline float3 SafeNormalize(float3 v)
{
    float lenSq = dot(v, v);
    return lenSq > 1e-6f ? normalize(v) : float3(0.0f, 0.0f, 1.0f);
}

inline VertexPositionInputs GetVertexPositionInputs(float3 positionOS)
{
    VertexPositionInputs output;
    
    output.positionWS = mul(float4(positionOS, 1.0f), worldMat);
    output.positionCS = mul(output.positionWS, viewProjMat);
    
    return output;
}

inline VertexPositionInputs GetVertexPositionInputs(float3 positionOS, matrix _worldMat)
{
    VertexPositionInputs output;
    
    output.positionWS = mul(float4(positionOS, 1.0f), _worldMat);
    output.positionCS = mul(output.positionWS, viewProjMat);
    
    return output;
}

inline VertexNormalInputs GetVertexNormalInputs(float3 normalOS, float3 tangentOS)
{
    VertexNormalInputs output;
    
    output.normalWS = normalize(mul(normalOS, (float3x3) invWorldMat));
    output.tangentWS = normalize(mul(tangentOS, (float3x3) invWorldMat));
    output.bitangentWS = normalize(cross(output.normalWS, output.tangentWS));
    
    return output;
}

inline VertexNormalInputs GetVertexNormalInputs(float3 normalOS, float3 tangentOS, matrix _invWorldMat)
{
    VertexNormalInputs output;
    
    output.normalWS = normalize(mul(normalOS, (float3x3) _invWorldMat));
    output.tangentWS = normalize(mul(tangentOS, (float3x3) _invWorldMat));
    output.bitangentWS = normalize(cross(output.normalWS, output.tangentWS));
    
    return output;
}

float4 ComputeScreenPos(float4 clipPos)
{
    float4 ndc = clipPos / clipPos.w;
    return float4(ndc.xy * 0.5f + 0.5f, ndc.z, clipPos.w);
}

float2 GetNormalizedScreenSpaceUV(float4 screenPos)
{
    float2 uv = screenPos.xy / renderTargetSize;
    return uv;
}

float GetNormalizedSceneDepth(float2 screenPos)
{
    //screenPos.y = 1.0 - screenPos.y;
    float depth = diffuseMap[gbufferDepthIdx].SampleLevel(anisoClamp, screenPos, 0).a;
    return depth;
}

float GetLinear01Depth(float z)
{
    float near = projectionParams.x;
    float far = projectionParams.y;
    return 1.0 / (1.0 / near + z * (1.0 / far - 1.0 / near));
}

float GetCameraDepth(float z)
{
    float nearZ = projectionParams.x;
    float farZ = projectionParams.y;
    return nearZ * farZ / ((farZ - nearZ) * z + nearZ);
}

inline float3 GammaDecoding(float3 color)
{
    return pow(color, 2.2f);
}

inline float3 GammaEncoding(float3 color)
{
    return pow(color, 0.4545f);
}

inline float Luminance(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

inline float3 ChangeLuminace(float3 color, float luminance)
{
    float luminanceColor = Luminance(color);
    return color * (luminance / luminanceColor);
}

inline float3 UnpackNormal(float3 normalMapSample, float scale = 1.f)
{
    float3 normal = 2.0f * normalMapSample - 1.0f;
    normal *= scale;
    return normalize(normal);
}

inline float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 normal, float3 tangent, float3 bitangent, float scale = 1.f)
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;
    normalT *= scale;
    normalT = normalize(normalT);

    float3x3 TBN = float3x3(tangent, bitangent, normal);

    return mul(normalT, TBN);
}

inline float3 UnpackedNormalSampleToWorldSpace(float3 normalMapSample, float3 normal, float3 tangent, float3 bitangent, float scale = 1.f)
{
    normalMapSample *= scale;
    normalMapSample = normalize(normalMapSample);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    
    return mul(normalMapSample, TBN);
}


float CalcShadowFactor(float4 shadowPosH)
{
    shadowPosH.xyz /= shadowPosH.w;

    if (shadowPosH.x < 0 || shadowPosH.x > 1 ||
    shadowPosH.y < 0 || shadowPosH.y > 1 ||
    shadowPosH.z < 0 || shadowPosH.z > 1)
        return 1.0f; // 그림자 바깥은 항상 밝게 처리
    
    // Depth in NDC space.
    float depth = shadowPosH.z - 0.001f;

    uint width, height, numMips;
    diffuseMap[shadowMapIdx].GetDimensions(0, width, height, numMips);
    
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
            shadowPosH.xy + offsets[i], depth ).r;
    }
    float shadowFactor = percentLit / 9.0f;
    if (shadowFactor < 0.3f)
        shadowFactor = 0.3f;
    else if (shadowFactor > 1.0f)
        shadowFactor = 1.0f;
    
    return shadowFactor;
}

inline float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

inline float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (3.14159 * denom * denom);
}

inline float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

inline float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

float3 ComputeDirectionalLight(LightingData lightingData, SurfaceData surfaceData, CBLightsData light)
{
    float3 normal = normalize(lightingData.normalWS);
    float3 camDir = normalize(lightingData.cameraDirection);
    float3 albedo = surfaceData.albedo;
    float metallic = surfaceData.metallic;
    float smoothness = clamp(surfaceData.smoothness, 0.0, 0.99f);
    float roughness = 1.0 - smoothness;
    float3 direction = light.directionWS;
    float3 lightDir = -normalize(direction);
    
    float3 lightColor = light.lColor * light.strength;

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);

    float3 halfV = normalize(camDir + lightDir);
    float NdotL = max((dot(normal, lightDir)), 0.0);
    float NdotV = max(dot(normal, camDir), 0.0);
    float VdotH = max(dot(camDir, halfV), 0.0);

    float3 F = FresnelSchlickRoughness(VdotH, F0, roughness);
    float NDF = DistributionGGX(normal, halfV, roughness);
    float G = GeometrySmith(normal, camDir, lightDir, roughness);

    float3 numerator = NDF * G * F;
    float denominator = max(4.0 * NdotL * NdotV, 0.00001);
    float3 specular = numerator / denominator;

    float3 kS = F;
    float3 kD = max(float3(1.f,1.f,1.f) - kS, 0.0) * (1.0 - metallic);
    float3 diffuse = albedo;

    float3 up = float3(0, 1, 0);
    float ndotUp = saturate(dot(normal, up));
    float3 directLight = (kD * albedo + specular) * lightColor * NdotL;
    float3 ambientLight = albedo * 0.2f * ndotUp;
    ambientLight += albedo * 0.3f;
    
    return (directLight + ambientLight) * lightingData.shadowFactor + surfaceData.emissive;
}


float3 ComputePointLight(LightingData lightingData, SurfaceData surfaceData, CBLightsData light)
{
    float3 normal = lightingData.normalWS;
    float3 position = lightingData.positionWS;
    float3 camDir = lightingData.cameraDirection;
    float3 albedo = surfaceData.albedo;
    float3 specular = surfaceData.specular;
    float metallic = surfaceData.metallic;
    float smoothness = surfaceData.smoothness;
    float3 lightDir = light.positionWS - position;
    float3 lightColor = light.lColor * light.strength;
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
    
    float3 F = FresnelSchlickRoughness(NdotV, F0, roughness);
    float NDF = DistributionGGX(normal, halfV, roughness);
    float G = GeometrySmith(normal, camDir, lightDir, roughness);
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.00001;
    specular *= (numerator / denominator);
    
    float3 kS = F;
    float3 kD = 1.0 - kS; 
    kD *= 1.0 - metallic; 
    float3 diffuse = albedo / 3.14159;
    float att = 1.0f / (a0 + distance * a1 + distance * distance * a2);
    
    return (kD * diffuse + specular) * lightColor * NdotL * att;
}


float3 ComputeSpotLight(LightingData lightingData, SurfaceData surfaceData, CBLightsData light)
{
    float3 normal = lightingData.normalWS;
    float3 position = lightingData.positionWS;
    float3 camDir = lightingData.cameraDirection;
    float3 albedo = surfaceData.albedo;
    float3 specular = surfaceData.specular;
    float metallic = surfaceData.metallic;
    float smoothness = surfaceData.smoothness;
    float3 lightDir = light.positionWS - position;
    float3 direction = light.directionWS;
    float3 lightColor = light.lColor * light.strength;
    float distanceSqr = dot(lightDir, lightDir);

// 거리 감쇠 계산
    
    lightDir = normalize(lightDir);
    
    if (distanceSqr > light.range * light.range)
        return float3(0.f, 0.f, 0.f);
    
    smoothness = clamp(surfaceData.smoothness, 0.0, 1.0);
    float roughness = 1.0 - smoothness;
    
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic); 
    
    float3 halfV = normalize(camDir + lightDir);
    float viewHalfDot = max(dot(halfV, normal), 0.f);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, camDir), 0.0);
    
    float3 F = FresnelSchlickRoughness(NdotV, F0, roughness);
    float NDF = DistributionGGX(normal, halfV, roughness);
    float G = GeometrySmith(normal, camDir, lightDir, roughness);
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.0001;
    specular *= (numerator / denominator);
    
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    float3 diffuse = albedo / 3.14159;
    
    float distanceAttenuation = saturate(1.0 - distanceSqr / (light.range * light.range));
    float fRho = dot(-lightDir, direction);
    float spotAtten = smoothstep(cos(light.spotAngle), cos(light.innerSpotAngle), fRho);
    float spotFactor = saturate(distanceAttenuation * spotAtten);
    
    return (kD * diffuse + specular) * lightColor * NdotL * spotFactor;
}

float3 CalculatePhongLight(LightingData lightingData, SurfaceData surfaceData)
{   
    float3 finalColor = float3(0.0, 0.0, 0.0);
    [unroll]
    for (int i = 0; i < lightCount; i++)
    {
        int lightType = lights[i].lightType;
        if (lightType == 0)
        {
            finalColor += ComputeDirectionalLight(lightingData, surfaceData, lights[i]);
        }
        else if (lightType == 1)
        {
            finalColor += ComputePointLight(lightingData, surfaceData, lights[i]);
        }
        else if (lightType == 2)
        {
            finalColor += ComputeSpotLight(lightingData, surfaceData, lights[i]);
        }
    }
    return finalColor;
}

#endif