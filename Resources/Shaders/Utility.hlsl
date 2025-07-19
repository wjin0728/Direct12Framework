// Utility.hlsl

#ifndef UTIL_DEFINE
#define UTIL_DEFINE

//#define USE_PCF

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
    float nearZ = projectionParams.x;
    float farZ = projectionParams.y;
    return 1.0 / (1.0 / nearZ + z * (1.0 / farZ - 1.0 / nearZ));
}

inline float GetCameraDepth(float z)
{
    float nearZ = projectionParams.x;
    float farZ = projectionParams.y;
    return nearZ * farZ / ((farZ - nearZ) * z + nearZ);
}

inline float3 GetScenePositionWS(float2 screenPos)
{
    return diffuseMap[gbufferPosIdx].SampleLevel(anisoClamp, screenPos, 0).xyz;
}

inline float3 GetSceneNormalWS(float2 screenPos)
{
    return diffuseMap[gbufferNormalIdx].SampleLevel(anisoClamp, screenPos, 0).xyz;
}

inline float3 GetScenePositionViewSpace(float2 screenPos)
{
    float3 positionWS = GetScenePositionWS(screenPos);
    return mul(float4(positionWS, 1.f), viewMat).xyz;
}

inline float3 GammaDecoding(float3 color)
{
    return pow(color, 2.2f);
}

inline float3 GammaEncoding(float3 color)
{
    return pow(color, 0.454545f);
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

static const float3x3 ACESInputMat = float3x3(
    0.59719, 0.35458, 0.04823,
    0.07600, 0.90834, 0.01566,
    0.02840, 0.13383, 0.83777
);

static const float3x3 ACESOutputMat = float3x3(
     1.60475, -0.53108, -0.07367,
    -0.10208, 1.10813, -0.00605,
    -0.00327, -0.07276, 1.07602
);


inline float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786) - 0.000090537;
    float3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return a / b;
}

inline float3 ToneMapping(float3 color)
{
    // 입력 색상 → ACEScg 색공간
    color = mul(ACESInputMat, color);
    // 톤매핑 커브
    color = RRTAndODTFit(color);
    // ACEScg → 출력 색공간(sRGB)
    color = mul(ACESOutputMat, color);
    // 필요에 따라 clamp(0,1) 적용 (옵션)
    return color;
}

inline float3 UnpackNormal(float3 normalMapSample, float scale = 1.f)
{
    float3 normal = 2.0f * normalMapSample - 1.0f;
    normal.xy *= scale;
    return normalize(normal);
}

inline float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 normal, float3 tangent, float3 bitangent, float scale = 1.f)
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;
    normalT.xy *= scale;
    normalT = normalize(normalT);

    float3x3 TBN = float3x3(tangent, bitangent, normal);

    return mul(normalT, TBN);
}

inline float3 UnpackedNormalSampleToWorldSpace(float3 normalMapSample, float3 normal, float3 tangent, float3 bitangent, float scale = 1.f)
{
    normalMapSample.xy *= scale;
    normalMapSample = normalize(normalMapSample);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    
    return mul(normalMapSample, TBN);
}

inline float CalPCFPercentLit(float4 shadowMapTexCoord, float rightTexelDepthWeight, float upTexelDepthWeight)
{
    float percentLit = 0.0f;
    
    const int PCF_BLUR_FOR_LOOP_SIZE = 3;
    int PCFBlurForLoopStart = -PCF_BLUR_FOR_LOOP_SIZE / 2;
    int PCFBlurForLoopEnd = PCF_BLUR_FOR_LOOP_SIZE / 2 + 1;
    
    for (int x = PCFBlurForLoopStart; x < PCFBlurForLoopEnd; ++x)
    {
        for (int y = PCFBlurForLoopStart; y < PCFBlurForLoopEnd; ++y)
        {
            float depthcompare = shadowMapTexCoord.z;
            depthcompare -= 0.001f;
            depthcompare += rightTexelDepthWeight * ((float) x) + upTexelDepthWeight * ((float) y);
            float2 finalShadowMapTexCoord = float2(
                shadowMapTexCoord.x + (((float) x) * nativeTexelSize),
                shadowMapTexCoord.y + (((float) y) * texelSize)
            );
            percentLit += diffuseMap[shadowMapTex].SampleCmpLevelZero(shadowSam, finalShadowMapTexCoord, depthcompare);
        }
    }
    
    return percentLit / (PCF_BLUR_FOR_LOOP_SIZE * PCF_BLUR_FOR_LOOP_SIZE);
}

float CalcShadowFactor(float4 shadowPosH)
{
    float4 shadowMapTexCoord = 0;
    float4 blendedShadowMapTexCoord = 0;
    
    int currentCascadeIdx = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        shadowMapTexCoord = shadowPosH * cascadeScale[i];
        shadowMapTexCoord += cascadeOffset[i];

        if (min(shadowMapTexCoord.x, shadowMapTexCoord.y) > minBorder && max(shadowMapTexCoord.x, shadowMapTexCoord.y) < maxBorder)
        {
            currentCascadeIdx = i;
            break;
        }
    }
    int nextCascadeIdx = min(cascadeCount - 1, currentCascadeIdx + 1);
    
    float2 distanceToOne = float2(1.0f - shadowMapTexCoord.x, 1.0f - shadowMapTexCoord.y);
    float currentPixelsBlendBandLocation = min(shadowMapTexCoord.x, shadowMapTexCoord.y);
    float currentPixelsBlendBandLocation2 = min(distanceToOne.x, distanceToOne.y);
    currentPixelsBlendBandLocation = min(currentPixelsBlendBandLocation, currentPixelsBlendBandLocation2);
    float blendAmount = currentPixelsBlendBandLocation / cascadeBlend;
    blendAmount = clamp(blendAmount, 0.0f, 1.0f);
    
    float4 shadowMapTexCoordDDX = ddx(shadowPosH) * cascadeScale[currentCascadeIdx];
    float4 shadowMapTexCoordDDY = ddy(shadowPosH) * cascadeScale[currentCascadeIdx];
    
    shadowMapTexCoord.x *= shadowPartition; 
    shadowMapTexCoord.x += (shadowPartition * (float) currentCascadeIdx);
        
    shadowPosH.xyz /= shadowPosH.w;
    
    float2x2 screentoShadowMat = float2x2(shadowMapTexCoordDDX.xy, shadowMapTexCoordDDY.xy);
    float invDeterminant = 1.0f / determinant(screentoShadowMat);
    
    float2x2 shadowToScreenMat = float2x2(
        screentoShadowMat._22 * invDeterminant, screentoShadowMat._12 * -invDeterminant,
        screentoShadowMat._21 * -invDeterminant, screentoShadowMat._11 * invDeterminant);

    float2 rightShadowTexelPos = float2(texelSize, 0.0f);
    float2 upShadowTexelPos = float2(0.0f, texelSize);
    
    float2 vRightTexelDepthRatio = mul(rightShadowTexelPos, shadowToScreenMat);
    float2 vUpTexelDepthRatio = mul(upShadowTexelPos, shadowToScreenMat);
    
    float upTexDepthWeight = vUpTexelDepthRatio.x * shadowMapTexCoordDDX.z + vUpTexelDepthRatio.y * shadowMapTexCoordDDY.z;
    float rightTexDepthWeight = vRightTexelDepthRatio.x * shadowMapTexCoordDDX.z + vRightTexelDepthRatio.y * shadowMapTexCoordDDY.z;
    
    float PCFPercentLit = CalPCFPercentLit(shadowMapTexCoord, rightTexDepthWeight, upTexDepthWeight);
    
    if (currentPixelsBlendBandLocation < cascadeBlend)
    {
        blendedShadowMapTexCoord = shadowPosH * cascadeScale[nextCascadeIdx];
        blendedShadowMapTexCoord += cascadeOffset[nextCascadeIdx];
        blendedShadowMapTexCoord.x *= shadowPartition;
        blendedShadowMapTexCoord.x += (shadowPartition * (float) nextCascadeIdx);
        
        //PCFPercentLit = lerp(PCFPercentLit, CalPCFPercentLit(blendedShadowMapTexCoord, rightTexDepthWeight, upTexDepthWeight), blendAmount);
    }
    
    float shadowFactor = PCFPercentLit;
    
    return PCFPercentLit;
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
    float smoothness = clamp(surfaceData.smoothness, 0.0, 1.0);
    float roughness = clamp(1 - smoothness, 0.14, 1.0);
    float3 direction = light.directionWS;
    float3 lightDir = -normalize(direction);
    
    light.lColor = GammaDecoding(light.lColor);
    //return camDir * 0.5f + 0.5f;
    
    float3 lightColor = light.lColor * 1.2;
    
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);

    float3 halfV = normalize(camDir + lightDir);
    if (all(halfV == 0))
        halfV = camDir;
    float NdotL = saturate((dot(normal, lightDir)));
    float NdotV = saturate(dot(normal, camDir));
    float VdotH = saturate(dot(camDir, halfV));
    //NdotL = smoothstep(0.0, 1.0, NdotL);

    float3 F = FresnelSchlickRoughness(VdotH, F0, roughness);
    float NDF = DistributionGGX(normal, halfV, roughness);
    float G = GeometrySmith(normal, camDir, lightDir, roughness);

    float3 numerator = NDF * G * F;
    float denominator = max(4.0 * NdotL * NdotV, 0.00001);
    float3 specular = numerator / denominator;

    float3 kS = F;
    float3 kD = max(1 - kS, 0.0) * (1.0 - metallic);

    float3 diffuse = albedo;

    float3 up = float3(0, 1, 0);
    float ndotUp = saturate(dot(normal, up));
    float3 directLight = (kD * albedo + specular) * lightColor * NdotL;
    float3 ambientLight = albedo * 0.1f * ndotUp;
    ambientLight += albedo * 0.35f;
    
    return ambientLight + (directLight * lightingData.shadowFactor) + surfaceData.emissive;
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
    
    return light.lColor;
    
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