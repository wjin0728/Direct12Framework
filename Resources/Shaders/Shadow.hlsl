#include"Paramiters.hlsl"
#include"Utility.hlsl"

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    float4 posW = mul(float4(input.pos, 1.f), worldMat);
    output.pos = mul(posW, viewProjMat);
    output.uv = mul(float4(input.uv, 0.f, 1.f), texMat).xy;

    return output;
}

//#define TRANSPARENT_CLIP

void PS_Main(VS_OUTPUT input)
{
    
#ifdef TRANSPARENT_CLIP
    // Discard pixel if texture alpha < 0.1.  We do this test as soon 
    // as possible in the shader so that we can potentially exit the
    // shader early, thereby skipping the rest of the shader code.
    Material material = materials[materialIdx];
    int diffuseMapIdx = material.diffuseMapIdx;
    
    float4 texColor = diffuseMap[diffuseMapIdx].Sample(anisoWrap, input.uv);
    clip(texColor.a - 0.1f);
    float4 color = float4(1.f, 0.f, 0.f, 1.f);
#endif
}