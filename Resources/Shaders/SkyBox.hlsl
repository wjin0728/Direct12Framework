#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float3 topColor;
    float offset;
    float3 bottomColor;
    float distance;
    
    float3 padding;
    float falloff;
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
    
    float2 uv : TEXCOORD1;
};

//¡§¡° ºŒ¿Ã¥ı
VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    
    output.position = output.position.xyww;
    
    output.uv = input.uv;
    
    return output;
}


//#define TRANSPARENT_CLIP

//«»ºø ºŒ¿Ã¥ı
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    float3 color = float3(0.4f, 0.4f, 0.4f);
    
    float3 worldPosition = input.positionWS.xyz;
    
    float2 uv = input.uv;
    
    float clampResult13 = clamp(((offset + worldPosition.y) / distance), 0.0, 1.0);
    float3 lerpResult18 = lerp(bottomColor, topColor, saturate(pow(clampResult13, falloff)));
    float2 texCoord21 = uv * float2(1, 1) + float2(0, 0);
    float3 lerpResult4 = lerp(bottomColor, topColor, texCoord21.y);
    
    float3 staticSwitch20 = lerpResult18;

    return float4(staticSwitch20, 1.f);
}
