#include"Paramiters.hlsl"
#include"Utility.hlsl"

struct ParticleVertex
{
    float3 position;
    float3 velocity;
    float rotation; 
    float4 color;
    float size;
    float distanceToCamera;
    int albedoTexIdx;
    int frameIdx;
    int tileX;
    int tileY;
};

StructuredBuffer<ParticleVertex> vertexBuffer : register(t0, space5);


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint texIdx : TEXCOORD1;
    nointerpolation float4 color : TEXCOORD2;
    nointerpolation float depth : TEXCOORD3;
    nointerpolation int2 tileSize : TEXCOORD4;
    nointerpolation float frameIdx : TEXCOORD5;
};

VS_OUTPUT VS_Forward(uint billboardVertex : SV_VertexID, uint instanceId : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    ParticleVertex input = vertexBuffer[instanceId];
    output.color = input.color;
    output.texIdx = input.albedoTexIdx;
    output.tileSize = int2(input.tileX, input.tileY);
    output.frameIdx = input.frameIdx;
    
    
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
    float2 sheetUV = input.uv;
    if (input.tileSize.x != 1 || input.tileSize.y != 1)
    {
        float2 tileSize = float2(1.0 / input.tileSize.x, 1.0 / input.tileSize.y);
        int frame = input.frameIdx;
        int frameX = frame % input.tileSize.x;
        int frameY = frame / input.tileSize.y;
    
        sheetUV = input.uv * tileSize + float2(frameX, frameY) * tileSize;
    }
    
    float4 texColor = diffuseMap[input.texIdx].Sample(linearClamp, sheetUV);
    texColor.rgb = GammaDecoding(texColor.rgb);
    color = color * texColor;
    
    float2 screenUV = GetNormalizedScreenSpaceUV(input.pos);
    float sceneDepth = GetNormalizedSceneDepth(screenUV);
    float linearSceneDepth = GetCameraDepth(sceneDepth);
    float linearFragmentDepth = GetCameraDepth(input.pos.z);
    
    color.a *= saturate(abs(linearFragmentDepth - linearSceneDepth) / 0.2f);
    
    return color;
    }