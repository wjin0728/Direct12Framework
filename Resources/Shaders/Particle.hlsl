#include"Paramiters.hlsl"

struct ParticleVertex
{
    float3 position;
    float4 color;
    float size;
    int albedoTexIdx;
};

StructuredBuffer<ParticleVertex> vertexBuffer : register(t0, space5);


struct VS_OUTPUT
{
    float4 pos : SV_Position;
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
    output.uv = float2((billboardVertex >> 1), (billboardVertex & 1));
    
    float2 corner = lerp(float2(-1, 1), float2(1, -1), output.uv);
    float3 position = mul((float3x3) invViewMat, float3(corner * input.size, 0)) + input.position;
    
    output.pos = float4(input.position, 1.0f);
    return output;
}

//#define TRANSPARENT_CLIP
float4 PS_Forward(VS_OUTPUT input) : SV_Target
{
    float4 color = input.color;
    float4 texColor = diffuseMap[input.texIdx].Sample(linearClamp, input.uv);
    return texColor * color;
}