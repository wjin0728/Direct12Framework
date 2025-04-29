#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float3 leafColor;
    float leafSmoothness;
    float leafMetallic;
    int leafTexIdx;
    int leafNormalIdx;
    float padding0;

    float3 trunkColor;
    float trunkSmoothness;
    float trunkMetallic;
    int trunkTexIdx = -1;
    int trunkNormalIdx = -1;
    float padding1;
};


//
//Forward
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 positionWS : TEXCOORD0;
    float3 normalWS : TEXCOORD1;
    float3 tangentWS : TEXCOORD2;
    float3 bitangentWS : TEXCOORD3;
    float4 ShadowPosH : TEXCOORD4;
    
    float4 color : COLOR;
    float2 uv : TEXCOORD5;
};

//Á¤Á¡ ¼ÎÀÌ´õ
VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(input.normal, input.tangent);
    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    
    output.normalWS = normalInputs.normalWS;
    output.tangentWS = normalInputs.tangentWS;
    output.bitangentWS = normalInputs.bitangentWS;
    
    output.ShadowPosH = mul(output.positionWS, shadowTransform);
        
    output.color = input.color;
    output.uv = input.uv;
    
    return output;
}


#define TRANSPARENT_CLIP

//ÇÈ¼¿ ¼ÎÀÌ´õ
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = input.tangentWS;
    float3 worldBitangent = input.bitangentWS;
    float2 uvLeaf = input.uv;
    float2 uvTrunk = input.uv;

    float4 texColor = (input.color.b > 0.5) ? diffuseMap[leafTexIdx].Sample(anisoClamp, uvLeaf) : diffuseMap[trunkTexIdx].Sample(anisoClamp, uvTrunk);
    texColor.rgb = GammaDecoding(texColor.rgb);
    
    color = max(texColor, float4(0.f,0.f,0.f, 0.f));
    
    #ifdef TRANSPARENT_CLIP
    clip(color.a - 0.5);
    #endif
    
    if (input.color.b > 0.5 && leafNormalIdx != -1)
    {
        float3 normalMapSample = diffuseMap[leafNormalIdx].Sample(anisoClamp, uvLeaf).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    else if (input.color.b <= 0.5 && trunkNormalIdx != -1)
    {
        float3 normalMapSample = diffuseMap[trunkNormalIdx].Sample(anisoClamp, uvTrunk).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    
    float3 camDir = (camPos - worldPosition);
    float distToEye = length(camDir);
    camDir /= distToEye;
    
    float temp = (1.0 - input.color.b);
    float lerpResult188 = lerp(leafMetallic, 0.0, temp);
    float lerpResult195 = lerp(trunkMetallic, 0.0, input.color.b);
				
    float lerpResult191 = lerp(leafSmoothness, 0.0, temp);
    float lerpResult190 = lerp(trunkSmoothness, 0.0, input.color.b);

    LightingData lightingData = (LightingData) 0;
    lightingData.cameraDirection = camDir;
    lightingData.normalWS = normal;
    lightingData.positionWS = worldPosition;
    lightingData.shadowFactor = CalcShadowFactor(input.ShadowPosH);
    
    SurfaceData surfaceData = (SurfaceData) 0;
    surfaceData.albedo = color.rgb;
    surfaceData.metallic = lerpResult188 + lerpResult195;
    surfaceData.smoothness = lerpResult191 + lerpResult190;
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

    return float4(color.rgb, 1.f);
}

//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Shadow Cast
struct VS_SHADOW_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VS_SHADOW_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

VS_SHADOW_OUTPUT VS_Shadow(VS_SHADOW_INPUT input)
{
    VS_SHADOW_OUTPUT output = (VS_SHADOW_OUTPUT) 0;
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
    
    output.position = positionInputs.positionCS;
    output.uv = input.uv;
    output.color = input.color;
    
    return output;
}

void PS_Shadow(VS_SHADOW_OUTPUT input)
{
    float2 uvLeaf = input.uv;
    float2 uvTrunk = input.uv;
    
    float4 texColor = (input.color.b > 0.5) ? diffuseMap[leafTexIdx].Sample(anisoClamp, uvLeaf) : diffuseMap[trunkTexIdx].Sample(anisoClamp, uvTrunk);
    float4 color = max(texColor, float4(leafColor, 0.f));
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.5);
#endif
}


//
//G Pass
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

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
        
    output.color = input.color;
    output.uv = input.uv;
    
    return output;
}

struct PS_GPASS_OUTPUT
{
    float4 albedo : SV_Target0;
    float4 normalWS : SV_Target1;
    float4 emissive : SV_Target2;
    float4 positionWS : SV_Target3;
    float4 depth : SV_Target4;
};

PS_GPASS_OUTPUT PS_GPass(VS_OUTPUT input) : SV_Target
{
    PS_GPASS_OUTPUT output = (PS_GPASS_OUTPUT) 0;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = input.tangentWS;
    float3 worldBitangent = input.bitangentWS;
    float2 uvLeaf = input.uv;
    float2 uvTrunk = input.uv;

    float4 texColor = (input.color.b > 0.5) ? diffuseMap[leafTexIdx].Sample(anisoClamp, uvLeaf) : diffuseMap[trunkTexIdx].Sample(anisoClamp, uvTrunk);
    
    color = max(texColor, float4(0.f, 0.f, 0.f, 0.f));
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.5);
#endif
    
    if (input.color.b > 0.5 && leafNormalIdx != -1)
    {
        float3 normalMapSample = diffuseMap[leafNormalIdx].Sample(anisoClamp, uvLeaf).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    else if (input.color.b <= 0.5 && trunkNormalIdx != -1)
    {
        float3 normalMapSample = diffuseMap[trunkNormalIdx].Sample(anisoClamp, uvTrunk).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    float temp = (1.0 - input.color.b);
    float lerpResult188 = lerp(leafMetallic, 0.0, temp);
    float lerpResult195 = lerp(trunkMetallic, 0.0, input.color.b);
				
    float lerpResult191 = lerp(leafSmoothness, 0.0, temp);
    float lerpResult190 = lerp(trunkSmoothness, 0.0, input.color.b);
    
    float shadowFactor = CalcShadowFactor(input.ShadowPosH);
    float depth = mul(input.positionWS, viewMat).z;
    
    color.rgb = GammaDecoding(color.rgb);
    
    output.albedo = color;
    output.normalWS = float4(normal, lerpResult188 + lerpResult195);
    output.positionWS = float4(worldPosition, lerpResult191 + lerpResult190);
    output.emissive = float4(0.f, 0.f, 0.f, shadowFactor);
    output.depth = input.ShadowPosH;
    
    return output;
}