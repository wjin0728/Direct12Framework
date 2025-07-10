#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float4 color;
    int decalTexIdx;
    float3 matPadding;
};

//
//Forward
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};

//¡§¡° ºŒ¿Ã¥ı
VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.position = mul(float4(input.position, 1.0f), worldMat);
    
    return output;
}

//«»ºø ºŒ¿Ã¥ı
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    
}
