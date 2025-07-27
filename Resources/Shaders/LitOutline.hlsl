#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float4 ForwardColor;
    uint ForwardTexIdx;
    
    uint normalTexIdx;
    float smoothness;
    float metallic;
    uint emissionMapIdx;
    float3 emissionColor;
};

//
//Forward
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
#ifdef USE_SKINNING
    float4 boneWeights : BONEWEIGHTS;
    uint4 boneIndices : BONEINDICES;
#endif
#ifdef USE_INSTANCING
    matrix worldMat : TRANSFORM;
	matrix invWorldMat : INVTRANSFORM;
	int idx0 : INDEX;
#endif
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 positionWS : TEXCOORD0;
    float4 positionCS : TEXCOORD1;
    float3 normal : NORMAL;
};

//¡§¡° ºŒ¿Ã¥ı
VS_OUTPUT VS_Forward(VS_INPUT input
#ifdef USE_INSTANCING
    , uint instanceId : SV_InstanceID
#endif
)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
#ifdef USE_SKINNING
    float3 position = float3(0.f, 0.f, 0.f);
    float3 normal = float3(0.f, 0.f, 0.f);

    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.boneWeights.x;
    weights[1] = input.boneWeights.y;
    weights[2] = input.boneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
    
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; ++i)
    {
        uint boneIndex = input.boneIndices[i];
        matrix boneTransform = mul(boneOffsets[boneIndex], boneTransforms[boneIndex]);
            
        position += weights[i] * mul(float4(input.position, 1.0), boneTransform).xyz;
        normal += weights[i] * mul(input.normal, (float3x3) boneTransform);
    }
#else
    float3 position = input.position;
    float3 normal = input.normal;
#endif
    
#ifdef USE_INSTANCING
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position, input.worldMat);
    normal = mul(normal, (float3x3) invWorldMat);
#elif USE_SKINNING
    VertexPositionInputs positionInputs;
    positionInputs.positionWS = float4(position, 1.0);
    positionInputs.positionCS = mul(positionInputs.positionWS, viewProjMat);
#else
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position);
    normal = mul(normal, (float3x3) invWorldMat);
#endif
    //output.normal = normalize(normal);
    
#ifdef USE_SKINNING

    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    output.positionCS = positionInputs.positionCS;
    
    
    return output;
}


//«»ºø ºŒ¿Ã¥ı
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    

    return color;
}
