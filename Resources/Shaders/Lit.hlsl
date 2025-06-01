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
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
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
    float3 normalWS : TEXCOORD2;
    float3 tangentWS : TEXCOORD3;
    float3 bitangentWS : TEXCOORD4;
    float4 ShadowPosH : TEXCOORD5;
    
    float2 uv : TEXCOORD6;
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
    float3 tangent = float3(0.f, 0.f, 0.f);

    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.boneWeights.x;
    weights[1] = input.boneWeights.y;
    weights[2] = input.boneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
    
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; ++i)
    {
        uint boneIndex = input.boneIndices[i];
        matrix boneTransform = boneTransforms[boneIndex];
            
        position += weights[i] * mul(float4(input.position, 1.0), boneTransform).xyz;
        normal += weights[i] * mul(input.normal, (float3x3) boneTransform);
        tangent += weights[i] * mul(input.tangent, (float3x3) boneTransform);
    }
#else
    float3 position = input.position;
    float3 normal = input.normal;
    float3 tangent = input.tangent;
#endif
    
#ifdef USE_INSTANCING
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position, input.worldMat);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(normal, tangent, invWorldMat);
#elif USE_SKINNING
    VertexPositionInputs positionInputs;
    VertexNormalInputs normalInputs;
    
    positionInputs.positionWS = float4(position, 1.0);
    positionInputs.positionCS = mul(positionInputs.positionWS, viewProjMat);
    normalInputs.normalWS = normal;
    normalInputs.tangentWS = tangent;
    normalInputs.bitangentWS = cross(normalInputs.normalWS, normalInputs.tangentWS);
#else
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(normal, tangent);
#endif
    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    output.positionCS = positionInputs.positionCS;
    output.normalWS = normalInputs.normalWS;
    output.tangentWS = normalInputs.tangentWS;
    output.bitangentWS = normalInputs.bitangentWS;
    
    output.ShadowPosH = mul(output.positionWS, shadowTransform);
    
    output.uv = input.uv;
    
    return output;
}


//«»ºø ºŒ¿Ã¥ı
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    float4 color = ForwardColor;
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = normalize(input.tangentWS);
    float3 worldBitangent = normalize(input.bitangentWS);
    
    float2 uv = input.uv;
    
    if (ForwardTexIdx != -1)
    {
        float4 texColor = diffuseMap[ForwardTexIdx].Sample(anisoClamp, uv);
        color *= texColor;
    }
    color.rgb = GammaDecoding(color.rgb);
    #ifdef TRANSPARENT_CLIP
    clip(color.a - 0.5);
    #endif
    
    if (normalTexIdx != -1)
    {
        float3 normalMapSample = diffuseMap[normalTexIdx].Sample(anisoClamp, uv).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    float3 _emissionColor = GammaDecoding(emissionColor);
    if (emissionMapIdx != -1)
    {
        float4 emissionColorSample = diffuseMap[emissionMapIdx].Sample(anisoClamp, uv);
        _emissionColor *= float4(GammaDecoding(emissionColorSample.rgb), emissionColorSample.a).rgb;
    }
    float3 camDir = (camPos - worldPosition);
    float distToEye = length(camDir);
    camDir /= distToEye;

    LightingData lightingData = (LightingData)0;
    lightingData.cameraDirection = camDir;
    lightingData.normalWS = normal;
    lightingData.positionWS = worldPosition;
    lightingData.shadowFactor = CalcShadowFactor(input.ShadowPosH);
    
    SurfaceData surfaceData = (SurfaceData)0;
    surfaceData.albedo = color.rgb;
    surfaceData.metallic = metallic;
    surfaceData.smoothness = smoothness;
    surfaceData.specular = 0.5f;
    surfaceData.emissive = _emissionColor;
    
#ifdef LIGHTING
    float3 finalColor = CalculatePhongLight(lightingData, surfaceData);
#else
    float3 finalColor = color.rgb;
#endif
    
#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
    color = lerp(color, gFogColor, fogAmount);
#endif

    return color;
}

//
//Shadow Cast
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#ifdef USE_SKINNING
struct VS_SHADOW_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 boneWeights : BONEWEIGHTS;
    uint4 boneIndices : BONEINDICES;
};

struct VS_SHADOW_OUTPUT
{
    float4 position : SV_POSITION;
};
#else
struct VS_SHADOW_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_SHADOW_OUTPUT
{
    float4 position : SV_POSITION;
};
#endif

VS_SHADOW_OUTPUT VS_Shadow(VS_SHADOW_INPUT input
#ifdef USE_INSTANCING
    , uint instanceId : SV_InstanceID
#endif
)
{
    VS_SHADOW_OUTPUT output = (VS_SHADOW_OUTPUT) 0;
    
#ifdef USE_SKINNING
    float3 position = float3(0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; ++i)
    {
        if (input.boneWeights[i] > 0.0)
        {
            uint boneIndex = input.boneIndices[i];
            matrix boneTransform = boneTransforms[boneIndex];
            float3 localPosition = mul(float4(input.position, 1.0), boneTransform).xyz;
            position += localPosition * input.boneWeights[i];
        }
    }
#else
    float3 position = input.position;
#endif
#ifdef USE_INSTANCING
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position, input.worldMat);
#elif USE_SKINNING
    VertexPositionInputs positionInputs;
    positionInputs.positionCS = mul(float4(position, 1.0), viewProjMat);
#else
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position);
#endif
    
    output.position = positionInputs.positionCS;
    
    return output;
}

void PS_Shadow(VS_SHADOW_OUTPUT input)
{
}

//
//G Pass
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct PS_GPASS_OUTPUT
{
    float4 albedo : SV_Target0;
    float4 normalWS : SV_Target1;
    float4 emissive : SV_Target2;
    float4 positionWS : SV_Target3;
    float4 depth : SV_Target4;
};

VS_OUTPUT VS_GPass(VS_INPUT input
#ifdef USE_INSTANCING
    , uint instanceId : SV_InstanceID
#endif
)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
#ifdef USE_SKINNING
    float3 position = float3(0.f, 0.f, 0.f);
    float3 normal = float3(0.f, 0.f, 0.f);
    float3 tangent = float3(0.f, 0.f, 0.f);

    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.boneWeights.x;
    weights[1] = input.boneWeights.y;
    weights[2] = input.boneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
    
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; ++i)
    {
        uint boneIndex = input.boneIndices[i];
        matrix boneTransform = boneTransforms[boneIndex];
            
        position += weights[i] * mul(float4(input.position, 1.0), boneTransform).xyz;
        normal += weights[i] * mul(input.normal, (float3x3) boneTransform);
        tangent += weights[i] * mul(input.tangent, (float3x3) boneTransform);
    }
#else
    float3 position = input.position;
    float3 normal = input.normal;
    float3 tangent = input.tangent;
#endif
    
#ifdef USE_INSTANCING
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position, input.worldMat);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(normal, tangent, input.invWorldMat);
#elif USE_SKINNING
    VertexPositionInputs positionInputs;
    VertexNormalInputs normalInputs;
    
    positionInputs.positionWS = float4(position, 1.0);
    positionInputs.positionCS = mul(positionInputs.positionWS, viewProjMat);
    
    normalInputs.normalWS = normal;
    normalInputs.tangentWS = tangent;
    normalInputs.bitangentWS = cross(normalInputs.normalWS, normalInputs.tangentWS);
#else
    VertexPositionInputs positionInputs = GetVertexPositionInputs(position);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(normal, tangent);
#endif
    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    output.positionCS = positionInputs.positionCS;
    
    output.normalWS = normalInputs.normalWS;
    output.tangentWS = normalInputs.tangentWS;
    output.bitangentWS = normalInputs.bitangentWS;
    
    matrix scaleMat = matrix(1.2f, 0.f, 0.f, 0.f,
                        0.f, 1.2f, 0.f, 0.f,
                        0.f, 0.f, 1.2f, 0.f,
                        0.f, 0.f, 0.f, 1.2f);
    
    float4 scaledPos = mul(scaleMat, float4(position, 1.f));
    output.ShadowPosH = mul(scaledPos, shadowTransform);
    
    output.uv = input.uv;
    
    return output;
}

PS_GPASS_OUTPUT PS_GPass(VS_OUTPUT input) : SV_Target
{
    PS_GPASS_OUTPUT output = (PS_GPASS_OUTPUT) 0;
    
    float4 color = ForwardColor;
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = normalize(input.tangentWS);
    float3 worldBitangent = normalize(input.bitangentWS);
    float2 uv = input.uv;
    
    
    if (ForwardTexIdx != -1)
    {
        float4 texColor = diffuseMap[ForwardTexIdx].Sample(anisoClamp, uv);
        color *= texColor;
    }
    color.rgb = GammaDecoding(color.rgb);
    
    if (normalTexIdx != -1)
    {
        float3 normalMapSample = diffuseMap[normalTexIdx].Sample(anisoClamp, uv).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    float3 _emissionColor = GammaDecoding(emissionColor);
    if (emissionMapIdx != -1)
    {
        float4 emissionColorSample = diffuseMap[emissionMapIdx].Sample(anisoClamp, uv);
        _emissionColor *= float4(GammaDecoding(emissionColorSample.rgb), emissionColorSample.a).rgb;
    }
    
    float shadowFactor = CalcShadowFactor(input.ShadowPosH);
    float depth = input.positionCS.z / input.positionCS.w;
    float3 hitColor = float3(1.0, 0.0, 0.0);
    _emissionColor = lerp(_emissionColor, hitColor, hitFactor);
    
    output.albedo = color;
    output.normalWS = float4(normal, metallic);
    output.emissive = float4(_emissionColor, shadowFactor);
    output.positionWS = float4(worldPosition, smoothness);
    output.depth = float4(0.f, 0.f, 0.f, depth);
    
    return output;
}