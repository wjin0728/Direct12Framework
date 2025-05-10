#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float4 ForwardColor;
    uint ForwardTexIdx;
    
    uint normalTexIdx;
    float smoothness;
    float metallic;
    uint emissionMapIdx;
    float3 emissionColor;
};

//
//Forward
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

#ifdef USE_SKINNING
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
#else
struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
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
#endif

//¡§¡° ºŒ¿Ã¥ı
VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
#ifdef USE_SKINNING
    float3 position = float3(0.f, 0.f, 0.f);
    float3 normal = float3(0.f, 0.f, 0.f);
    float3 tangent = float3(0.f, 0.f, 0.f);

    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.boneWeights.x;
    weights[1] = input.boneWeights.y;
    weights[2] = input.boneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
    
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; ++i)
    {
        uint boneIndex = input.boneIndices[i];
        matrix boneTransform = boneTransforms[boneIndex];
            
        position += weights[i] * mul(float4(input.position, 1.0), boneTransform).xyz;
        normal += weights[i] * mul(input.normal, (float3x3) boneTransform);
        tangent += weights[i] * mul(input.tangent, (float3x3) boneTransform);
    }
#else
    float3 position = input.position;
    float3 normal = input.normal;
    float3 tangent = input.tangent;
#endif
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(normal, tangent);
    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    
    output.normalWS = normalInputs.normalWS;
    output.tangentWS = normalInputs.tangentWS;
    output.bitangentWS = normalInputs.bitangentWS;
    
    output.ShadowPosH = mul(output.positionWS, shadowTransform);
    
    output.uv = input.uv;
    
    return output;
}


//«»ºø ºŒ¿Ã¥ı
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    float4 color = ForwardColor;
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = input.tangentWS;
    float3 worldBitangent = input.bitangentWS;
    
    float2 uv = input.uv;
    
    if (ForwardTexIdx != -1)
    {
        float4 texColor = diffuseMap[ForwardTexIdx].Sample(anisoClamp, uv);
        color *= float4(GammaDecoding(texColor.rgb), texColor.a);
    }
    
    
    if (normalTexIdx != -1)
    {
        float3 normalMapSample = diffuseMap[normalTexIdx].Sample(anisoClamp, uv).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    float3 _emissionColor = emissionColor;
    if (emissionMapIdx != -1)
    {
        float4 emissionColorSample = diffuseMap[emissionMapIdx].Sample(anisoClamp, uv);
        _emissionColor *= float4(GammaDecoding(emissionColorSample.rgb), emissionColorSample.a).rgb;
    }
    float3 camDir = (camPos - worldPosition);
    float distToEye = length(camDir);
    camDir /= distToEye;

    LightingData lightingData = (LightingData)0;
    lightingData.cameraDirection = camDir;
    lightingData.normalWS = normal;
    lightingData.positionWS = worldPosition;
    lightingData.shadowFactor = CalcShadowFactor(input.ShadowPosH);
    
    SurfaceData surfaceData = (SurfaceData)0;
    surfaceData.albedo = color.rgb;
    surfaceData.metallic = metallic;
    surfaceData.smoothness = smoothness;
    surfaceData.specular = 0.5f;
    surfaceData.emissive = _emissionColor;
    
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

    return float4(1.f, 1.f, 1.f, 0.5f);
}

//
//Shadow Cast
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#ifdef USE_SKINNING
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
#else
struct VS_SHADOW_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_SHADOW_OUTPUT
{
    float4 position : SV_POSITION;
};
#endif

VS_SHADOW_OUTPUT VS_Shadow(VS_SHADOW_INPUT input)
{
    VS_SHADOW_OUTPUT output = (VS_SHADOW_OUTPUT) 0;
    
#ifdef USE_SKINNING
    float3 position = float3(0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; ++i)
    {
        if (input.boneWeights[i] > 0.0)
        {
            uint boneIndex = input.boneIndices[i];
            matrix boneTransform = boneTransforms[boneIndex];
            float3 localPosition = mul(float4(input.position, 1.0), boneTransform).xyz;
            position += localPosition * input.boneWeights[i];
        }
    }
#else
    float3 position = input.position;
#endif
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position);
    
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
    float4 depth : SV_Target4;
};

VS_OUTPUT VS_GPass(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
#ifdef USE_SKINNING
    float3 position = float3(0.f, 0.f, 0.f);
    float3 normal = float3(0.f, 0.f, 0.f);
    float3 tangent = float3(0.f, 0.f, 0.f);

    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.boneWeights.x;
    weights[1] = input.boneWeights.y;
    weights[2] = input.boneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
    
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; ++i)
    {
        uint boneIndex = input.boneIndices[i];
        matrix boneTransform = boneTransforms[boneIndex];
            
        position += weights[i] * mul(float4(input.position, 1.0), boneTransform).xyz;
        normal += weights[i] * mul(input.normal, (float3x3) boneTransform);
        tangent += weights[i] * mul(input.tangent, (float3x3) boneTransform);
    }
#else
    float3 position = input.position;
    float3 normal = input.normal;
    float3 tangent = input.tangent;
#endif
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(normal, tangent);
    
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
    
    float4 color = ForwardColor;
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = input.tangentWS;
    float3 worldBitangent = input.bitangentWS;
    float2 uv = input.uv;
    
    
    if (ForwardTexIdx != -1)
    {
        float4 texColor = diffuseMap[ForwardTexIdx].Sample(anisoClamp, uv);
        color*= float4(GammaDecoding(texColor.rgb), texColor.a);
    }
    
    
    if (normalTexIdx != -1)
    {
        float3 normalMapSample = diffuseMap[normalTexIdx].Sample(anisoClamp, uv).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    float3 _emissionColor = emissionColor;
    if (emissionMapIdx != -1)
    {
        float4 emissionColorSample = diffuseMap[emissionMapIdx].Sample(anisoClamp, uv);
        _emissionColor *= float4(GammaDecoding(emissionColorSample.rgb), emissionColorSample.a).rgb;
    }
    
    float shadowFactor = CalcShadowFactor(input.ShadowPosH);
    float depth = mul(input.positionWS, viewMat).z;
    
    output.albedo = color;
    output.normalWS = float4(normal, metallic);
    output.emissive = float4(_emissionColor, shadowFactor);
    output.positionWS = float4(worldPosition, smoothness);
    output.depth = float4(0.f, 0.f, 0.f, depth);
    
    return output;
}