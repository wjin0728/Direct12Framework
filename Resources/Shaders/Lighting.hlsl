#include"Paramiters.hlsl"
#include"Utility.hlsl"

float4 VS_Final(uint vertexID : SV_VertexID) : SV_Position
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