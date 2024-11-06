#include"Paramiters.hlsl"
#include"Utility.hlsl"


struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 localPos : POSITION;
};

//¡§¡° ºŒ¿Ã¥ı
VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.localPos = input.position;
    
    
    float4 worldPos = mul(float4(output.localPos, 1.0f), worldMat);
    worldPos.xyz += camPos;
    
    output.position = mul(worldPos, viewProjMat).xyww;
    
    return output;
}


#define TRANSPARENT_CLIP


float4 PS_Main(VS_OUTPUT input) : SV_TARGET
{
    return skyBoxMap.Sample(linearWrap, input.localPos);
}
