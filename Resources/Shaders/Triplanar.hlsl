#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float fallOff;
    float tilling;
    float padding;
    
    int topTexIdx;
    int topNormalIdx;
    int sideTexIdx;
    int sideNormalIdx;
};


float4 TriplanarSampling(Texture2D topTexMap, Texture2D midTexMap, Texture2D botTexMap, float3 worldPos, float3 worldNormal, float falloff, float tiling)
{
    float3 projNormal = pow(abs(worldNormal), falloff);
    projNormal /= (projNormal.x + projNormal.y + projNormal.z) + 0.00001;
    float3 nsign = sign(worldNormal);
    float negProjNormalY = max(0, projNormal.y * -nsign.y);
    projNormal.y = max(0, projNormal.y * nsign.y);
    float4 xNorm;
    float4 yNorm;
    float4 yNormN;
    float4 zNorm;
    
    xNorm = midTexMap.Sample(anisoWrap, float2(tiling, tiling) * worldPos.zy * float2(nsign.x, 1.0));
    yNorm = topTexMap.Sample(anisoWrap, tiling * worldPos.xz * float2(nsign.y, 1.0));
    yNormN = botTexMap.Sample(anisoWrap, tiling * worldPos.xz * float2(nsign.y, 1.0));
    zNorm = midTexMap.Sample(anisoWrap, tiling * worldPos.xy * float2(-nsign.z, 1.0));
    
    return xNorm * projNormal.x + yNorm * projNormal.y + yNormN * negProjNormalY + zNorm * projNormal.z;
}


//
//Forward
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct VS_INPUT
{
    float3 position : POSITION;
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
    
    return output;
}


#define TRANSPARENT_CLIP

//«»ºø ºŒ¿Ã¥ı
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 0.f, 0.f, 1.f);
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = input.tangentWS;
    float3 worldBitangent = input.bitangentWS;
    float4 texColor = TriplanarSampling(diffuseMap[topTexIdx], diffuseMap[sideTexIdx], diffuseMap[sideTexIdx], worldPosition, worldNormal, fallOff, tilling);
    color = float4(GammaDecoding(texColor.rgb), texColor.a);
    
    #ifdef TRANSPARENT_CLIP
    clip(color.a - 0.1);
    #endif
    
    float3 camDir = (camPos - input.positionWS.xyz);
    float distToEye = length(camDir);
    camDir /= distToEye;
   
    
    if (topNormalIdx != -1 && sideNormalIdx != -1)
    {
        float3 normalMapSample = TriplanarSampling(diffuseMap[topNormalIdx], diffuseMap[sideNormalIdx], diffuseMap[sideNormalIdx], worldPosition, worldNormal, fallOff, tilling).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    
    LightingData lightingData = (LightingData)0;
    lightingData.cameraDirection = camDir;
    lightingData.normalWS = normal;
    lightingData.positionWS = worldPosition;
    lightingData.shadowFactor = CalcShadowFactor(input.ShadowPosH);
    
    SurfaceData surfaceData = (SurfaceData)0;
    surfaceData.albedo = color.rgb;
    surfaceData.metallic = 0.f;
    surfaceData.smoothness = 0.f;
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Shadow Cast
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
    float4 depth : SV_Target4;
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
    
    return output;
}

PS_GPASS_OUTPUT PS_GPass(VS_OUTPUT input) : SV_Target
{
    PS_GPASS_OUTPUT output = (PS_GPASS_OUTPUT) 0;
    
    float4 color = float4(1.f, 0.f, 0.f, 1.f);
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = input.tangentWS;
    float3 worldBitangent = input.bitangentWS;
    float4 texColor = TriplanarSampling(diffuseMap[topTexIdx], diffuseMap[sideTexIdx], diffuseMap[sideTexIdx], worldPosition, worldNormal, fallOff, tilling);
    color = float4(GammaDecoding(texColor.rgb), texColor.a);
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.1);
#endif
    if (topNormalIdx != -1 && sideNormalIdx != -1)
    {
        float3 normalMapSample = TriplanarSampling(diffuseMap[topNormalIdx], diffuseMap[sideNormalIdx], diffuseMap[sideNormalIdx], worldPosition, worldNormal, fallOff, tilling).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    
    float shadowFactor = CalcShadowFactor(input.ShadowPosH);
    float depth = mul(input.positionWS, viewMat).z;
    
    output.albedo = color;
    output.normalWS = float4(normal, 0.f);
    output.emissive = float4(0.f, 0.f, 0.f, shadowFactor);
    output.positionWS = float4(worldPosition, 0.f);
    output.depth = input.ShadowPosH;
    
    return output;
}