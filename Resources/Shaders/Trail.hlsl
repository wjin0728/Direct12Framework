#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float4 ForwardColor;
    uint ForwardTexIdx;
    
    uint normalTexIdx;
    float smoothness;
    float metallic;
    
    float4 vec4Data0;
    float4 vec4Data1;
    float fData0;
    float fData1;
    float fData2;
    float fData3;
    int iData0;
    int iData1;
    int iData2;
    int iData3;
};

struct VS_INPUT
{
    float3 position : POSITION;
    uint texIdx : TEXCOORD;
    float2 uv : TIME;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.color = input.color;
    output.pos = mul(float4(input.position, 1), viewProjMat);
    return output;
}

float4 PS_Forward(VS_OUTPUT input) : SV_Target
{
    float4 color = input.color;
    float4 texColor = diffuseMap[ForwardTexIdx].Sample(linearClamp, input.uv);
    texColor = float4(1.0, 1.0, 1.0, 1.0);
    color = color * texColor;
    color.rgb = GammaDecoding(color.rgb);
    return color;
}