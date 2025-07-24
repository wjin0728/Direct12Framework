#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float4 ForwardColor;
    int ForwardTexIdx;
    
    int normalTexIdx;
    float smoothness;
    float metallic;
    
    float4 vec4Data0;
    float4 vec4Data1;
    float fData0;
    float fData1;
    float2 tilling;
    int iData0;
    int iData1;
    int iData2;
    int iData3;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float time : TIME;
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
    output.uv = input.uv;
    return output;
}

float4 PS_Forward(VS_OUTPUT input) : SV_Target
{
    float4 color = input.color;
    float4 texColor = float4(1.0, 1.0, 1.0, 1.0);
    float alpha = 1.0;
    if (normalTexIdx >= 0)
    {
        alpha = diffuseMap[normalTexIdx].Sample(linearClamp, input.uv).r;
    }
    
    float2 screenUV = GetNormalizedScreenSpaceUV(input.pos);
    float sceneDepth = GetNormalizedSceneDepth(screenUV);
    float linearSceneDepth = GetCameraDepth(sceneDepth);
    float linearFragmentDepth = GetCameraDepth(input.pos.z);
    
    float depth = saturate((linearSceneDepth - linearFragmentDepth) / 0.2);
    
    
    color = color * texColor;
    color.a *= alpha;
    //color.rgb = GammaDecoding(color.rgb);
    return color;
}