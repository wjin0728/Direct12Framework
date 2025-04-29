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
    float3 emissive;
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

float4 ComputeScreenPos(float4 clipPos)
{
    float4 o = clipPos;
  
    o.xy = clipPos.xy / clipPos.w;
    o.xy = o.xy * 0.5f + 0.5f;
    
    o.z = clipPos.z / clipPos.w;
    o.w = clipPos.w;

    return o;
}

float3 GammaDecoding(float3 color)
{
    return pow(color, 2.2f);
}

float3 GammaEncoding(float3 color)
{
    return pow(color, 0.4545f);
}

float Luminance(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 normal, float3 tangent, float3 bitangent)
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    float3x3 TBN = float3x3(tangent, bitangent, normal);

    return mul(normalT, TBN);
}

float CalcShadowFactor(float4 shadowPosH)
{
    shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

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
    
    return percentLit / 9.0f;
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(1.0 - roughness, F0) - F0) * pow(1.0 - cosTheta, 5.0);
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

float3 ComputeDirectionalLight(LightingData lightingData, SurfaceData surfaceData)
{
    float3 normal = normalize(lightingData.normalWS);
    float3 camDir = normalize(lightingData.cameraDirection);
    float3 albedo = surfaceData.albedo;
    float metallic = surfaceData.metallic;
    float smoothness = clamp(surfaceData.smoothness, 0.0, 1.0);
    float roughness = 1.0 - smoothness;
    float3 direction = { 0.62, -0.64, 0.41 };
    float3 lightDir = -normalize(direction);
    float3 lightColor = lColor * lightingData.shadowFactor;

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);

    float3 halfV = normalize(camDir + lightDir);
    if (all(halfV == 0)) halfV = camDir;
    float NdotL = max((dot(normal, lightDir) * 0.5f) + 0.5f, 0.0);
    float NdotV = max(dot(normal, camDir), 0.0);
    float VdotH = max(dot(camDir, halfV), 0.0);

    float3 F = FresnelSchlickRoughness(VdotH, F0, roughness);
    float NDF = DistributionGGX(normal, halfV, roughness);
    float G = GeometrySmith(normal, camDir, lightDir, roughness);

    float3 numerator = NDF * G * F;
    float denominator = max(4.0 * NdotL * NdotV, 0.00001);
    float3 specular = numerator / denominator;

    float3 kS = F;
    float3 kD = max(1.0 - kS, 0.0) * (1.0 - metallic);
    float3 diffuse = albedo;

    float3 up = float3(0, 1, 0);
    float ndotUp = saturate(dot(normal, up));
    float3 directLight = (kD * diffuse + specular)  * NdotL;
    float3 ambientLight = albedo * 0.3f * ndotUp;
    
    return directLight;
}


float3 ComputePointLight(LightingData lightingData, SurfaceData surfaceData)
{
    float3 normal = lightingData.normalWS;
    float3 position = lightingData.positionWS;
    float3 camDir = lightingData.cameraDirection;
    float3 albedo = surfaceData.albedo;
    float3 specular = surfaceData.specular;
    float metallic = surfaceData.metallic;
    float smoothness = surfaceData.smoothness;
    float3 lightDir = lightMat._41_42_43 - position;
    float3 lightColor = lColor * strength;
    float distance = length(lightDir);
    
    lightDir = normalize(lightDir);
    
    if (distance > range)
        return float3(0.f, 0.f, 0.f);
    
    smoothness = clamp(surfaceData.smoothness, 0.0, 1.0);
    float roughness = 1.0 - smoothness;
    float3 F0 = float3(0.04, 0.04, 0.04); // ��ݼ� �⺻��
    F0 = lerp(F0, albedo, metallic); // �ݼӼ� �ݿ�
    
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


float3 ComputeSpotLight(LightingData lightingData, SurfaceData surfaceData)
{
    float3 normal = lightingData.normalWS;
    float3 position = lightingData.positionWS;
    float3 camDir = lightingData.cameraDirection;
    float3 albedo = surfaceData.albedo;
    float3 specular = surfaceData.specular;
    float metallic = surfaceData.metallic;
    float smoothness = surfaceData.smoothness;
    float3 lightDir = lightMat._41_42_43 - position;
    float3 direction = lightMat._31_32_33;
    float3 lightColor = lColor * strength;
    float distanceSqr = dot(lightDir, lightDir);

// �Ÿ� ���� ���
    
    lightDir = normalize(lightDir);
    
    if (distanceSqr > range * range)
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
    
    float distanceAttenuation = saturate(1.0 - distanceSqr / (range * range));
    float fRho = dot(-lightDir, direction);
    float spotAtten = smoothstep(cos(spotAngle), cos(innerSpotAngle), fRho);
    float spotFactor = saturate(distanceAttenuation * spotAtten);
    
    return (kD * diffuse + specular) * lightColor * NdotL * spotFactor;
}

float3 CalculatePhongLight(LightingData lightingData, SurfaceData surfaceData)
{   
    return surfaceData.albedo;
}

#endif