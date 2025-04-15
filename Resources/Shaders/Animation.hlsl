#include"Paramiters.hlsl"
#include"Utility.hlsl"

#define MAX_VERTEX_INFLUENCES 4

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

//¡§¡° ºŒ¿Ã¥ı
VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    float3 skinnedPosition = float3(0.0, 0.0, 0.0);
    float3 skinnedNormal = float3(0.0, 0.0, 0.0);
    float3 skinnedTangent = float3(0.0, 0.0, 0.0);
    
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.boneWeights.x;
    weights[1] = input.boneWeights.y;
    weights[2] = input.boneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    for (int i = 0; i < MAX_VERTEX_INFLUENCES; ++i)
    {
        uint boneIndex = input.boneIndices[i];
        matrix boneTransform = boneTransforms[boneIndex];
            
        skinnedPosition += weights[i] * mul(float4(input.position, 1.0), boneTransform).xyz;
        skinnedNormal += weights[i] * mul(input.normal, (float3x3) boneTransform);
        skinnedTangent += weights[i] * mul(input.tangent, (float3x3) boneTransform);
    }
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(skinnedPosition);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(skinnedNormal, skinnedTangent);
    
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

//«»ºø ºŒ¿Ã¥ı
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
    float4 boneWeights : BONEWEIGHTS;
    uint4 boneIndices : BONEINDICES;
};

struct VS_SHADOW_OUTPUT
{
    float4 position : SV_POSITION;
};

VS_SHADOW_OUTPUT VS_Shadow(VS_SHADOW_INPUT input)
{
    VS_SHADOW_OUTPUT output = (VS_SHADOW_OUTPUT) 0;
    
    float3 skinnedPosition = float3(0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; ++i)
    {
        if (input.boneWeights[i] > 0.0)
        {
            uint boneIndex = input.boneIndices[i];
            matrix boneTransform = boneTransforms[boneIndex];
            float3 localPosition = mul(float4(input.position, 1.0), boneTransform).xyz;
            skinnedPosition += localPosition * input.boneWeights[i];
        }
    }

    VertexPositionInputs positionInputs = GetVertexPositionInputs(skinnedPosition);
    
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
    
    float3 skinnedPosition = float3(0.0, 0.0, 0.0);
    float3 skinnedNormal = float3(0.0, 0.0, 0.0);
    float3 skinnedTangent = float3(0.0, 0.0, 0.0);
    
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; ++i)
    {
        if (input.boneWeights[i] > 0.0)
        {
            uint boneIndex = input.boneIndices[i];
            matrix boneTransform = boneTransforms[boneIndex];
            
            float3 localPosition = mul(float4(input.position, 1.0), boneTransform).xyz;
            skinnedPosition += localPosition * input.boneWeights[i];
            
            float3 localNormal = mul(input.normal, (float3x3)boneTransform);
            skinnedNormal += localNormal * input.boneWeights[i];
            
            float3 localTangent = mul(input.tangent, (float3x3)boneTransform);
            skinnedTangent += localTangent * input.boneWeights[i];
        }
    }

    VertexPositionInputs positionInputs = GetVertexPositionInputs(skinnedPosition);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(skinnedNormal, skinnedTangent);
    
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