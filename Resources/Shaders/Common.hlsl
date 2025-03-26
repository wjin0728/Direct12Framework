#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b0, space1)
{
    uint ForwardTexIdx;
    float3 ForwardColor;
    
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
    
    float4 texColor = diffuseMap[ForwardTexIdx].Sample(anisoClamp, uv);;
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
    
    output.position = positionInputs.positionWS;
    
    return output;
}

void PS_Shadow(VS_SHADOW_OUTPUT input)
{
   
}