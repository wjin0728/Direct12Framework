#include"Paramiters.hlsl"
#include"Utility.hlsl"

float4 VS_FinalPass(uint vertexID : SV_VertexID) : SV_Position
{
    float2 pos[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0, 3.0),
        float2(3.0, -1.0)
    };
    
    return float4(pos[vertexID], 0.0, 1.0);
}

float4 PS_FinalPass(float4 position : SV_Position) : SV_Target
{
    float2 uv = position.xy / renderTargetSize;
    float4 color = diffuseMap[lightingTargetIdx].SampleLevel(pointClamp, uv, 0.0);
    color.rgb = GammaEncoding(color.rgb);
    return float4(color.rgb, finalRenderTargetAlpha.a);
}