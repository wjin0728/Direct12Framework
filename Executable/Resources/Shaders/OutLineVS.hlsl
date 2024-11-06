struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
};

cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
    Material material : packoffset(c4);
};

cbuffer CBPassData : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 camPos : packoffset(c8);
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};





//정점 셰이더를 정의한다.
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    matrix scaleMat =
    {
        1.05f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.05f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.05f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
    };
    
    float4 worldPos = mul(float4(input.position, 1.0f), scaleMat);
    
    output.position =  mul(mul(mul(worldPos, gmtxWorld), gmtxView), gmtxProjection);
    

    return output;
}

