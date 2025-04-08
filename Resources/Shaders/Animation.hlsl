#include"Paramiters.hlsl"
#include"Utility.hlsl"

#define MAX_VERTEX_INFLUENCES 4

void ApplyBoneTransform(float3 position, float3 normal, float3 tangent, uint4 boneIndices, float4 boneWeights,
                        out float3 outPosition, out float3 outNormal, out float3 outTangent)
{
    outPosition = float3(0.0f, 0.0f, 0.0f);
    outNormal = float3(0.0f, 0.0f, 0.0f);
    outTangent = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
    {
        // 뼈대 오프셋과 변환 행렬을 결합
        matrix vertexToBoneWorld = mul(boneOffsets[boneIndices[i]], boneTransforms[boneIndices[i]]);
        outPosition += boneWeights[i] * mul(float4(position, 1.0f), vertexToBoneWorld).xyz;
        outNormal += boneWeights[i] * mul(normal, (float3x3)vertexToBoneWorld);
        outTangent += boneWeights[i] * mul(tangent, (float3x3)vertexToBoneWorld);
    }
}

cbuffer MaterialData : register(b5)
{
    float3 ForwardColor;
    uint ForwardTexIdx;
    
    uint normalTexIdx;
    float smoothness;
    float metallic;
    float padding;
};



//
//Forward
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 boneWeights : BONEWEIGHTS;
    uint4 boneIndices : BONEINDICES;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 positionWS : TEXCOORD0;
    float3 normalWS : TEXCOORD1;
    float3 tangentWS : TEXCOORD2;
    float3 bitangentWS : TEXCOORD3;
    float4 ShadowPosH : TEXCOORD4;
    
    float2 uv : TEXCOORD5;
};

//정점 셰이더
VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(input.normal, input.tangent);
    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    
    output.normalWS = normalInputs.normalWS;
    output.tangentWS = normalInputs.tangentWS;
    output.bitangentWS = normalInputs.bitangentWS;
    
    output.ShadowPosH = mul(output.positionWS, shadowTransform);
    
    output.uv = input.uv;
    
    return output;
}


#define TRANSPARENT_CLIP

//픽셀 셰이더
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = input.tangentWS;
    float3 worldBitangent = input.bitangentWS;
    
    float2 uv = input.uv;
    
    float4 texColor = diffuseMap[ForwardTexIdx].Sample(anisoClamp, uv);
    color = float4(GammaDecoding(texColor.rgb), texColor.a);
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.1);
#endif
    
    float3 camDir = (camPos - worldPosition);
    float distToEye = length(camDir);
    camDir /= distToEye;
    
    if (normalTexIdx != -1)
    {
        float3 normalMapSample = diffuseMap[normalTexIdx].Sample(anisoClamp, uv).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }

    LightingData lightingData = (LightingData) 0;
    lightingData.cameraDirection = camDir;
    lightingData.normalWS = normal;
    lightingData.positionWS = worldPosition;
    lightingData.shadowFactor = CalcShadowFactor(input.ShadowPosH);
    
    SurfaceData surfaceData = (SurfaceData) 0;
    surfaceData.albedo = color.rgb;
    surfaceData.metallic = metallic;
    surfaceData.smoothness = smoothness;
    surfaceData.specular = 0.5f;
    surfaceData.emissive = 0.f;
    
#ifdef LIGHTING
    float3 finalColor = CalculatePhongLight(lightingData, surfaceData);
#else
    float3 finalColor = color.rgb;
#endif
    
    color.xyz = GammaEncoding(finalColor);
    
#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
    color = lerp(color, gFogColor, fogAmount);
#endif

    return color;
}

//
//Shadow Cast
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SHADOW_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_SHADOW_OUTPUT
{
    float4 position : SV_POSITION;
};

VS_SHADOW_OUTPUT VS_Shadow(VS_SHADOW_INPUT input)
{
    VS_SHADOW_OUTPUT output = (VS_SHADOW_OUTPUT) 0;
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
    
    output.position = positionInputs.positionCS;
    
    return output;
}

void PS_Shadow(VS_SHADOW_OUTPUT input)
{
}

//
//G Pass
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct PS_GPASS_OUTPUT
{
    float4 albedo : SV_Target0;
    float4 normalWS : SV_Target1;
    float4 emissive : SV_Target2;
    float4 positionWS : SV_Target3;
};

VS_OUTPUT VS_GPass(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(input.normal, input.tangent);
    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    
    output.normalWS = normalInputs.normalWS;
    output.tangentWS = normalInputs.tangentWS;
    output.bitangentWS = normalInputs.bitangentWS;
    
    output.ShadowPosH = mul(output.positionWS, shadowTransform);
    
    output.uv = input.uv;
    
    return output;
}

PS_GPASS_OUTPUT PS_GPass(VS_OUTPUT input) : SV_Target
{
    PS_GPASS_OUTPUT output = (PS_GPASS_OUTPUT) 0;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = input.tangentWS;
    float3 worldBitangent = input.bitangentWS;
    float2 uv = input.uv;
    
    float4 texColor = diffuseMap[ForwardTexIdx].Sample(anisoClamp, uv);
    color = float4(GammaDecoding(texColor.rgb), texColor.a);
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.1);
#endif
    if (normalTexIdx != -1)
    {
        float3 normalMapSample = diffuseMap[normalTexIdx].Sample(anisoClamp, uv).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    
    float shadowFactor = CalcShadowFactor(input.ShadowPosH);
    
    output.albedo = color;
    output.normalWS = float4(normal, metallic);
    output.positionWS = float4(worldPosition, smoothness);
    output.emissive = float4(0.f, 0.f, 0.f, shadowFactor);
    
    return output;
}