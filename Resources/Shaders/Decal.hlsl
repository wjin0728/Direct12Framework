#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float4 ForwardColor;
    uint ForwardTexIdx;
    
    uint normalTexIdx;
    float smoothness;
    float metallic;
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
    float2 screenUV = GetNormalizedScreenSpaceUV(input.position);
    
    float3 scenePosWS = GetScenePositionWS(screenUV);
    if (scenePosWS.x <= -99999.f) discard;
    
    invWorldMat = transpose(invWorldMat);
    float3 localPos = mul(float4(scenePosWS, 1.0f), invWorldMat).xyz;
    
    if (localPos.x < -0.5f || localPos.x > 0.5f ||
        localPos.y < -0.5f || localPos.y > 0.5f ||
        localPos.z < -0.5f || localPos.z > 0.5f)
    {
        discard;
    }
    
    float4 texColor = diffuseMap[ForwardTexIdx].Sample(anisoClamp, localPos.xy + 0.5f); // Sample the texture using the UV coordinates
    return texColor * ForwardColor;
}
