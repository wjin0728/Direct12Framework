#include"Paramiters.hlsl"
#include"Utility.hlsl"

struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};

float4 VS_Directional(uint vertexID : SV_VertexID) : SV_Position
{
    float2 pos[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0, 3.0),
        float2(3.0, -1.0)
    };
    
    return float4(pos[vertexID], 1.0, 1.0);
}

[earlydepthstencil]
float4 PS_Directional(float4 position : SV_Position) : SV_Target
{
    float2 uv = position.xy / renderTargetSize;
    float4 color = diffuseMap[gbufferAlbedoIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferNormal = diffuseMap[gbufferNormalIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferEmissive = diffuseMap[gbufferEmissiveIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferPosition = diffuseMap[gbufferPosIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferDepth = diffuseMap[gbufferDepthIdx].SampleLevel(pointClamp, uv, 0.0);
    
    float3 positionWS = gbufferPosition.xyz;
    float3 normal = gbufferNormal.rgb;
    float metallic = gbufferNormal.a;
    float smoothness = gbufferPosition.a;
    float3 emissive = gbufferEmissive.rgb;
    float shadow = gbufferEmissive.a;
    
    float3 camDir = (camPos - positionWS);
    float distToEye = length(camDir);
    camDir /= distToEye;

    LightingData lightingData = (LightingData) 0;
    lightingData.cameraDirection = camDir;
    lightingData.normalWS = normal;
    lightingData.positionWS = positionWS;
    lightingData.shadowFactor = shadow;
    
    SurfaceData surfaceData = (SurfaceData) 0;
    surfaceData.albedo = color.rgb;
    surfaceData.metallic = metallic;
    surfaceData.smoothness = smoothness;
    surfaceData.specular = 0.5f;
    surfaceData.emissive = 0.f;
    
    float3 finalColor = { 0.f, 0.f, 0.f };                                          
    finalColor = ComputeDirectionalLight(lightingData, surfaceData);
    
    finalColor = GammaEncoding(finalColor);
    
    return float4(finalColor, 1.f);
}

VS_OUTPUT VS_Lighting(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
    output.position = positionInputs.positionCS;
    return output;
}

[earlydepthstencil]
float4 PS_Lighting(VS_OUTPUT output) : SV_Target
{
    float2 uv = output.position.xy / renderTargetSize;
    float4 color = diffuseMap[1].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferNormal = diffuseMap[gbufferNormalIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferEmissive = diffuseMap[gbufferEmissiveIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferPosition = diffuseMap[gbufferPosIdx].SampleLevel(pointClamp, uv, 0.0);
    float4 gbufferDepth = diffuseMap[gbufferDepthIdx].SampleLevel(pointClamp, uv, 0.0);
    
    float3 positionWS = gbufferPosition.xyz;
    float3 normal = gbufferNormal.rgb;
    float metallic = gbufferNormal.a;
    float smoothness = gbufferNormal.a;
    float3 emissive = gbufferEmissive.rgb;
    float shadow = gbufferEmissive.a;
    
    float3 camDir = (camPos - positionWS);
    float distToEye = length(camDir);
    camDir /= distToEye;

    LightingData lightingData = (LightingData) 0;
    lightingData.cameraDirection = camDir;
    lightingData.normalWS = normal;
    lightingData.positionWS = positionWS;
    lightingData.shadowFactor = shadow;
    
    SurfaceData surfaceData = (SurfaceData) 0;
    surfaceData.albedo = color.rgb;
    surfaceData.metallic = metallic;
    surfaceData.smoothness = smoothness;
    surfaceData.specular = 0.5f;
    surfaceData.emissive = 0.f;
    
    float3 finalColor = { 0.f, 0.f, 0.f };
    if (lightType == 1)
    {
        finalColor = ComputePointLight(lightingData, surfaceData);
    }
    else if (lightType == 2)
    {
        finalColor = ComputeSpotLight(lightingData, surfaceData);
    }
    else
    {
        finalColor = color.rgb;
    }
    
    finalColor = GammaEncoding(finalColor);
    
    return float4(finalColor, 1.f);
}

VS_OUTPUT VS_Stencil(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    if (lightType == 0)
    {
        output.position = float4(input.position.x, input.position.y, 0.0f, 1.0f);
    }
    else
    {
        VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
        output.position = positionInputs.positionCS;
    }
    return output;
}

[earlydepthstencil]
void PS_Stencil(VS_OUTPUT output)
{
}