#include"Paramiters.hlsl"
#include"Utility.hlsl"


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
    float4 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float2 uv : TEXCOORD;
    float4 ShadowPosH : POSITION1;
};

//¡§¡° ºŒ¿Ã¥ı
VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.worldPos = mul(float4(input.position, 1.0f), worldMat);
    output.position = mul(output.worldPos, viewProjMat);
    
    output.worldNormal = mul((input.normal), (float3x3) invWorldMat);
    output.worldTangent = mul((input.tangent), (float3x3) invWorldMat);
    output.uv = input.uv;
    
    output.ShadowPosH = mul(output.worldPos, shadowTransform);
    
    return output;
}


#define TRANSPARENT_CLIP

//«»ºø ºŒ¿Ã¥ı
[earlydepthstencil]
float4 PS_Main(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    Material material = materials[materialIdx];
    int diffuseMapIdx = material.diffuseMapIdx;
    int normalMapIdx = material.normalMapIdx;
    
    float4 texColor = diffuseMap[diffuseMapIdx].Sample(anisoWrap, input.uv);
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
