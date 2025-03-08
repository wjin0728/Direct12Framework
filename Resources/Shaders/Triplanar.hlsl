#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b0, space1)
{
    float fallOff;
    float2 tilling;
    float padding;
    
    uint topTexIdx;
    uint topNormalIdx;
    uint sideTexIdx;
    uint sideNormalIdx;
};


float4 TriplanarSampling(Texture2D topTexMap, Texture2D midTexMap, Texture2D botTexMap, float3 worldPos, float3 worldNormal, float falloff, float2 tiling)
{
    float3 projNormal = (pow(abs(worldNormal), falloff));
    projNormal /= (projNormal.x + projNormal.y + projNormal.z) + 0.00001;
    float3 nsign = sign(worldNormal);
    float negProjNormalY = max(0, projNormal.y * -nsign.y);
    projNormal.y = max(0, projNormal.y * nsign.y);
    float4 xNorm;
    float4 yNorm;
    float4 yNormN;
    float4 zNorm;
    
    xNorm = midTexMap.Sample(anisoWrap, tiling * worldPos.zy * float2(nsign.x, 1.0));
    yNorm = topTexMap.Sample(anisoWrap,tiling * worldPos.xz * float2(nsign.y, 1.0));
    yNormN = botTexMap.Sample(anisoWrap, tiling * worldPos.xz * float2(nsign.y, 1.0));
    zNorm = midTexMap.Sample(anisoWrap, tiling * worldPos.xy * float2(-nsign.z, 1.0));
    
    return xNorm * projNormal.x + yNorm * projNormal.y + yNormN * negProjNormalY + zNorm * projNormal.z;
}


struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : TEXCOORD0;
    float3 worldNormal : TEXCOORD1;
    float3 worldTangent : TEXCOORD2;
    float3 worldBitangent : TEXCOORD3;
    float4 ShadowPosH : TEXCOORD4;
};

//¡§¡° ºŒ¿Ã¥ı
VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.worldPos = mul(float4(input.position, 1.0f), worldMat);
    output.position = mul(output.worldPos, viewProjMat);
    
    output.worldNormal = mul((input.normal), (float3x3) invWorldMat);
    output.worldTangent = mul((input.tangent), (float3x3) invWorldMat);
    
    output.ShadowPosH = mul(output.worldPos, shadowTransform);
    
    return output;
}


#define TRANSPARENT_CLIP

//«»ºø ºŒ¿Ã¥ı
float4 PS_Main(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    float4 texColor = TriplanarSampling(diffuseMap[topTexIdx], diffuseMap[sideTexIdx], diffuseMap[topTexIdx], input.worldPos, input.worldNormal, fallOff, tilling);
    color = float4(GammaDecoding(texColor.rgb), texColor.a);
    
    #ifdef TRANSPARENT_CLIP
    clip(color.a - 0.1);
    #endif
    
    float3 normal = normalize(input.worldNormal);
    float3 camDir = (camPos - input.worldPos.xyz);
    float distToEye = length(camDir);
    camDir /= distToEye;
    
    if (normalMapIdx != -1)
    {
        float4 normalMapSample = diffuseMap[normalMapIdx].Sample(anisoWrap, input.uv);
        normal = NormalSampleToWorldSpace(normalMapSample.rgb, normal, input.worldTangent);
    }

    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    shadowFactor[0] = CalcShadowFactor(input.ShadowPosH);
    
    LightColor finalColor = CalculatePhongLight(input.position.xyz, normal, camDir, material, shadowFactor);
    
    color.xyz = GammaEncoding((finalColor.diffuse.xyz * color.xyz) + finalColor.specular.xyz + (0.1 * color.xyz));
    
#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
    color = lerp(color, gFogColor, fogAmount);
#endif

    return float4(color.xyz, 1.f);
}
