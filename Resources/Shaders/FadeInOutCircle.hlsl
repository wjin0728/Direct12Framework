#include"Paramiters.hlsl"
#include"Utility.hlsl"


float4 VS_FadeInOutCircle(uint vertexID : SV_VertexID) : SV_Position
{
    float2 pos[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0, 3.0),
        float2(3.0, -1.0)
    };
    
    return float4(pos[vertexID], 0.0, 1.0);
}

float4 PS_FadeInOutCircle(float4 position : SV_Position) : SV_Target
{
    float2 uv = position.xy / renderTargetSize;
    float2 center = float2(0.5, 0.5);
    float dist = distance(uv, center);

    float mask = smoothstep(fadeInOutColor.w, fadeInOutColor.w - 0.1f, dist);
    float alpha = 1.0 - mask; 
    return float4(0, 0, 0, alpha);
}