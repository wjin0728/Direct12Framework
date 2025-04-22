#include"Paramiters.hlsl"
#include"Utility.hlsl"

float4 VS_Directional(uint vertexID : SV_VertexID) : SV_Position
{
    float2 pos[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0, 3.0),
        float2(3.0, -1.0)
    };
    
    return float4(pos[vertexID], 0.0, 1.0);
}

float4 PS_Final(float4 position : SV_Position) : SV_Target
{
    float2 uv = position.xy * 0.5f + 0.5f;
    float4 color = diffuseMap[finalTargetIdx].SampleLevel(pointClamp, uv, 0.0);
    return color;
}

float4 PS_Directional(float4 position : SV_Position) : SV_Target
{
    float2 uv = position.xy / renderTargetSize;
    float4 color = diffuseMap[gbufferAlbedoIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferNormal = diffuseMap[gbufferNormalIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferEmissive = diffuseMap[gbufferEmissiveIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferPosition = diffuseMap[gbufferPosIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferDepth = diffuseMap[gbufferDepthIdx].SampleLevel(pointClamp, uv, 0.0);
    
    float3 positionWS = gbufferPosition.xyz;
    float3 normal = gbufferNormal.rgb;
    float metallic = gbufferNormal.a;
    float smoothness = gbufferNormal.a;
    float3 emissive = gbufferEmissive.rgb;
    float shadow = gbufferEmissive.a;
    
   
    return color;
}