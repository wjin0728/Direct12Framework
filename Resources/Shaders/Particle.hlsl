#include"Paramiters.hlsl"

struct ParticleVertex
{
    float3 position;
    float rotation; 
    float4 color;
    float size;
    float distanceToCamera;
    int albedoTexIdx;
};

StructuredBuffer<ParticleVertex> vertexBuffer : register(t0, space5);


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint texIdx : TEXCOORD1;
    nointerpolation float4 color : TEXCOORD2;
    nointerpolation float depth : TEXCOORD3;
};

VS_OUTPUT VS_Forward(uint billboardVertex : SV_VertexID, uint instanceId : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    ParticleVertex input = vertexBuffer[instanceId];
    output.color = input.color;
    output.texIdx = input.albedoTexIdx;
    
    float2 corner = float2(0, 0);
   
    output.uv = float2((billboardVertex >> 1), (billboardVertex & 1));
    
    corner = lerp(float2(-0.5, 0.5), float2(0.5, -0.5), output.uv) * input.size;
    corner = float2(
        corner.x * cos(input.rotation) - corner.y * sin(input.rotation),
        corner.x * sin(input.rotation) + corner.y * cos(input.rotation)
    );
    float3 position = mul(float3(corner, 0), (float3x3) invViewMat) + input.position;
    
    output.pos = mul(float4(position, 1), viewProjMat);
    return output;
}

float4 PS_Forward(VS_OUTPUT input) : SV_Target
{
    float4 color = input.color;
    float4 texColor = diffuseMap[input.texIdx].Sample(linearClamp, input.uv);
    color = color * texColor;
    clip(color.a - 0.001);
    return color;
}