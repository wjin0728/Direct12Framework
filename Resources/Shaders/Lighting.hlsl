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
    float2 uv : TEXCOORD;
};

//¡§¡° ºŒ¿Ã¥ı
VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    
    output.worldPos = mul(float4(input.position, 1.0f), worldMat);
    output.position = mul(output.worldPos, viewProjMat);
    output.worldNormal = mul((input.normal), (float3x3) worldMat);
    output.uv = input.uv;
    
    return output;
}


#define TRANSPARENT_CLIP


//«»ºø ºŒ¿Ã¥ı
float4 PS_Main(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    if (materialIdx == -1)
        return color;
    
    int idx = materials[materialIdx].diffuseMapIdx;
    if (idx == -1)
        return materials[materialIdx].albedo;
    
    Material mat = materials[materialIdx];
    color = diffuseMap[mat.diffuseMapIdx].Sample(pointClamp, input.uv);
    #ifdef TRANSPARENT_CLIP
    clip(color.a - 0.1);
    #endif
    
    float3 normal = normalize(input.worldNormal);
    float3 camDir = normalize(camPos - input.worldPos.xyz);
    
    LightColor finalColor = CalculatePhongLight(input.position.xyz, normal, camDir, mat);
    
    color.xyz = (finalColor.diffuse.xyz * color.xyz) + finalColor.specular.xyz;
    
    return float4(color.xyz, 1.f);
}
