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
};



//¡§¡° ºŒ¿Ã¥ı∏¶ ¡§¿««—¥Ÿ.
VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.worldPos = float4(input.position, 1.0f);
    output.position = mul(output.worldPos, viewProjMat);
    output.worldNormal = input.normal;
    output.worldTangent = input.tangent;
    output.uv = input.uv;
    
    return output;
}

#define TRANSPARENT_CLIP

//«»ºø ºŒ¿Ã¥ı
float4 PS_Main(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    Material mat = terrainMat.material;
    
    float4 texColor = diffuseMap[mat.diffuseMapIdx].Sample(pointWrap, input.uv);
    color = float4(GammaDecoding(texColor.rgb), texColor.a);
    
    if (terrainMat.detailMapTdx != -1)
    {
        float4 detailColor = diffuseMap[terrainMat.detailMapTdx].Sample(linearWrap, input.uv * 50.f);
   
        color = lerp(color, float4(GammaDecoding(detailColor.rgb), detailColor.a), 0.5);
    }
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.1);
#endif
    
    float3 normal = normalize(input.worldNormal);
    float3 camDir = normalize(camPos - input.worldPos.xyz);
    
    LightColor finalColor = CalculatePhongLight(input.position.xyz, normal, camDir, mat);
    
    color.xyz = GammaEncoding((finalColor.diffuse.xyz * color.xyz) + finalColor.specular.xyz + (0.05 * color.xyz));

    return float4(color.xyz, 1.f);
}