#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b0, space1)
{
    float3 topColor;
    float3 bottomColor;
    
    float offset;
    float distance;
    float falloff;
    
    float3 padding;
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
    
    float3 botcol = { 1.f, 1.f, 1.f };
    float3 topcol = { 0.f, 0.6f, 0.8f };
    
    float fall = 0.8;
    float off = 69.2f;
    float dis = 148.f;
    
    float clampResult13 = clamp(((off + worldPosition.y) / dis), 0.0, 1.0);
    float3 lerpResult18 = lerp(botcol, topcol, saturate(pow(clampResult13, fall)));
    float2 texCoord21 = uv * float2(1, 1) + float2(0, 0);
    float3 lerpResult4 = lerp(botcol, topcol, texCoord21.y);
    
    float3 staticSwitch20 = lerpResult18;

    return float4(staticSwitch20, 1.f);
}

//
//Shadow Cast
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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
    
    output.position = positionInputs.positionWS;
    
    return output;
}

void PS_Shadow(VS_SHADOW_OUTPUT input)
{
   
}