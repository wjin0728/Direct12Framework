#include"Paramiters.hlsl"

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.pos = mul(float4(input.pos, 1.f), worldMat);
    output.uv = mul(float4(input.uv, 0.f, 1.f), texMat).xy;

    return output;
}

float4 PS_Forward(VS_OUTPUT input) : SV_Target
{
    float4 color = float4(1.f, 0.f, 0.f, 1.f);
    
    
    
    return color;
}