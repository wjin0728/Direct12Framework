#include"Paramiters.hlsl"
#include"Utility.hlsl"

//#define DYNAMIC_TESSELLATION

struct SplatData
{
    float4 data[4];
};

cbuffer MaterialData : register(b3)
{
    float3 size;
    float yOffset;
    
    int heightMapIdx;
    int splatNum;
    float2 heightMapResolution;
    
    SplatData splats[TERRAIN_SPLAT_COUNT];
    float4 alphaMapIdx[TERRAIN_SPLAT_COUNT];
};

float CalculateTessFactor(float3 position)
{
    float d = distance(position, camPos);
    
    const float dMin = 100.0f;
    const float dMax = 1000.0f;
    float s = 64.f *saturate((dMax - d) / (dMax - dMin));
    
    return s;
}

VertexNormalInputs CalculateTerrainNormal(float2 uv, Texture2D heightMap)
{
    float2 texelSize = 1.0 / heightMapResolution;

    float heightCenter = heightMap.SampleLevel(anisoClamp, uv, 0).r;
    float heightLeft = heightMap.SampleLevel(anisoClamp, uv - float2(texelSize.x, 0), 0).r;
    float heightRight = heightMap.SampleLevel(anisoClamp, uv + float2(texelSize.x, 0), 0).r;
    float heightDown = heightMap.SampleLevel(anisoClamp, uv - float2(0, texelSize.y), 0).r;
    float heightUp = heightMap.SampleLevel(anisoClamp, uv + float2(0, texelSize.y), 0).r;

    float heightScale = size.y;
    float dx = (heightRight - heightLeft) * heightScale / (2.0 * texelSize.x);
    float dz = (heightUp - heightDown) * heightScale / (2.0 * texelSize.y);

    VertexNormalInputs result = (VertexNormalInputs) 0;
    result.tangentWS = normalize(float3(1.0, dx, 0.0)); // 원래 방식 유지
    result.bitangentWS = normalize(float3(0.0, dz, -1.0));
    result.normalWS = normalize(cross(result.tangentWS, result.bitangentWS));
    
    return result;
}


struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};


//정점 셰이더를 정의한다.
VS_INPUT VS_Forward(VS_INPUT input)
{   
    return input;
}

struct TessFactor
{
    float EdgeTess[4] : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

TessFactor ConstantHS(InputPatch<VS_INPUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    TessFactor pt;
	
    float3 edge0 = 0.5f * (patch[0].position + patch[2].position);
    float3 edge1 = 0.5f * (patch[0].position + patch[1].position);
    float3 edge2 = 0.5f * (patch[1].position + patch[3].position);
    float3 edge3 = 0.5f * (patch[2].position + patch[3].position);
    float3 center = 0.25f * (patch[0].position + patch[1].position + patch[2].position + patch[3].position);
	
 #ifdef DYNAMIC_TESSELLATION
    pt.EdgeTess[0] = CalculateTessFactor(edge0);
    pt.EdgeTess[1] = CalculateTessFactor(edge1);
    pt.EdgeTess[2] = CalculateTessFactor(edge2);
    pt.EdgeTess[3] = CalculateTessFactor(edge3);
    
    pt.InsideTess[0] = CalculateTessFactor(center);
    pt.InsideTess[1] = pt.InsideTess[0];
 #else
	pt.EdgeTess[0] = 32.f;
    pt.EdgeTess[1] = 32.f;
    pt.EdgeTess[2] = 32.f;
    pt.EdgeTess[3] = 32.f;
    
    pt.InsideTess[0] = 32.f;
    pt.InsideTess[1] = 32.f;
#endif
    
    return pt;
}

struct HS_OUTPUT
{
    float3 PosL : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HS_OUTPUT HS_Forward(InputPatch<VS_INPUT, 4> p, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HS_OUTPUT hout;
	
    hout.PosL = p[i].position;
    hout.uv = p[i].uv;
    hout.normal = p[i].normal;
    hout.tangent = p[i].tangent;
	
    return hout;
}

struct DS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float4 ShadowPosH : TEXCOORD2;
};

[domain("quad")]
DS_OUTPUT DS_Forward(TessFactor tessFactors, float2 uv : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 4> quad)
{
    DS_OUTPUT dout = (DS_OUTPUT)0;
	
    float3 v1 = lerp(quad[0].PosL, quad[1].PosL, uv.x);
    float3 v2 = lerp(quad[2].PosL, quad[3].PosL, uv.x);
    float3 p = lerp(v1, v2, uv.y);
    
    float2 uv1 = lerp(quad[0].uv, quad[1].uv, uv.x);
    float2 uv2 = lerp(quad[2].uv, quad[3].uv, uv.x);
    dout.uv = lerp(uv1, uv2, uv.y);
	
    Texture2D heightMap = diffuseMap[heightMapIdx];
    p.y = heightMap.SampleLevel(linearClamp, dout.uv, 0).r * size.y + yOffset;
    
    dout.worldPos = float4(p, 1.0f);
    dout.position = mul(dout.worldPos, viewProjMat);
    dout.ShadowPosH = mul(dout.worldPos, shadowTransform);
	
    return dout;
}

#define TRANSPARENT_CLIP

//픽셀 셰이더
float4 PS_Forward(DS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(0.f, 0.f, 0.f, 1.f);
    float2 uv = input.uv;
    float3 positionWS = input.worldPos.xyz;
    float2 diffuseUV = input.uv * 15.f;
    
    VertexNormalInputs normalInputs = CalculateTerrainNormal(uv, diffuseMap[heightMapIdx]);
    float3 normalWS = normalInputs.normalWS;
    float3 tangentWS = normalInputs.tangentWS;
    float3 bitangentWS = normalInputs.bitangentWS;
    float3 normal = normalWS;
    float3 blendedNormal = float3(0.f, 0.f, 0.f);
    float blendedMetallic = 0.f;
    float blendedSmoothness = 0.f;
    
    float totalWeight = 0.0;
    [unroll(TERRAIN_SPLAT_COUNT)]
    for (int i = 0; i < splatNum; i++)
    {
        SplatData splat = splats[i];
        float4 weight = diffuseMap[alphaMapIdx[i].x].Sample(anisoClamp, uv);
        
        color.rgb += diffuseMap[splat.data[0].x].Sample(anisoWrap, diffuseUV).rgb * weight.r;
        color.rgb += diffuseMap[splat.data[1].x].Sample(anisoWrap, diffuseUV).rgb * weight.g;
        color.rgb += diffuseMap[splat.data[2].x].Sample(anisoWrap, diffuseUV).rgb * weight.b;
        color.rgb += diffuseMap[splat.data[3].x].Sample(anisoWrap, diffuseUV).rgb * weight.a;
        
        blendedNormal += (diffuseMap[splat.data[0].y].Sample(anisoWrap, diffuseUV).xyz * 2.0 - 1.0) * weight.r;
        blendedNormal += (diffuseMap[splat.data[1].y].Sample(anisoWrap, diffuseUV).xyz * 2.0 - 1.0) * weight.g;
        blendedNormal += (diffuseMap[splat.data[2].y].Sample(anisoWrap, diffuseUV).xyz * 2.0 - 1.0) * weight.b;
        blendedNormal += (diffuseMap[splat.data[3].y].Sample(anisoWrap, diffuseUV).xyz * 2.0 - 1.0) * weight.a;
        
        blendedMetallic += splat.data[0].z * weight.r;
        blendedMetallic += splat.data[1].z * weight.g;
        blendedMetallic += splat.data[2].z * weight.b;
        blendedMetallic += splat.data[3].z * weight.a;
        
        blendedSmoothness += splat.data[0].w * weight.r;
        blendedSmoothness += splat.data[1].w * weight.g;
        blendedSmoothness += splat.data[2].w * weight.b;
        blendedSmoothness += splat.data[3].w * weight.a;
        
        totalWeight += weight.r + weight.g + weight.b + weight.a;

    }
    
    if (totalWeight > 0.0)
    {
        color.rgb /= totalWeight;
        blendedNormal /= totalWeight;
        blendedMetallic /= totalWeight;
        blendedSmoothness /= totalWeight;
    }
    else
    {
        blendedNormal = float3(0, 0, 1);
    }
    blendedNormal = normalize(blendedNormal);
    
    normal = normalize(mul(blendedNormal, float3x3(tangentWS, bitangentWS, normalWS)));
   
    
    color.rgb = GammaDecoding(color.rgb);
   
    float3 camDir = (camPos - input.worldPos.xyz);
    float distToEye = length(camDir);
    camDir /= distToEye;
    
    LightingData lightingData = (LightingData) 0;
    lightingData.cameraDirection = camDir;
    lightingData.normalWS = normal;
    lightingData.positionWS = input.worldPos.xyz;
    lightingData.shadowFactor = CalcShadowFactor(input.ShadowPosH);
    
    SurfaceData surfaceData = (SurfaceData) 0;
    surfaceData.albedo = color.rgb;
    surfaceData.specular = 0.5f;
    surfaceData.smoothness = blendedSmoothness;
    surfaceData.metallic = blendedMetallic;
    surfaceData.emissive = 0.f;
    
    float3 finalColor = CalculatePhongLight(lightingData, surfaceData);
    
    color.xyz = GammaEncoding(finalColor);
    
    #ifdef FOG
    float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
    color = lerp(color, gFogColor, fogAmount);
#endif

    return float4(color.xyz, 1.f);
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
};

PS_GPASS_OUTPUT PS_GPass(VS_GPASS_OUTPUT input) : SV_Target
{
    PS_GPASS_OUTPUT output = (PS_GPASS_OUTPUT) 0;
    
    float4 color = float4(0.f, 0.f, 0.f, 1.f);
    float2 uv = input.uv;
    float3 positionWS = input.worldPos.xyz;
    float2 diffuseUV = input.uv * 15.f;
    
    VertexNormalInputs normalInputs = CalculateTerrainNormal(uv, diffuseMap[heightMapIdx]);
    float3 normalWS = normalInputs.normalWS;
    float3 tangentWS = normalInputs.tangentWS;
    float3 bitangentWS = normalInputs.bitangentWS;
    float3 normal = normalWS;
    float3 blendedNormal = float3(0.f, 0.f, 0.f);
    float blendedMetallic = 0.f;
    float blendedSmoothness = 0.f;
    
    float totalWeight = 0.0;
    [unroll(TERRAIN_SPLAT_COUNT)]
    for (int i = 0; i < splatNum; i++)
    {
        SplatData splat = splats[i];
        float4 weight = diffuseMap[alphaMapIdx[i].x].Sample(anisoClamp, uv);
        
        color.rgb += diffuseMap[splat.data[0].x].Sample(anisoWrap, diffuseUV).rgb * weight.r;
        color.rgb += diffuseMap[splat.data[1].x].Sample(anisoWrap, diffuseUV).rgb * weight.g;
        color.rgb += diffuseMap[splat.data[2].x].Sample(anisoWrap, diffuseUV).rgb * weight.b;
        color.rgb += diffuseMap[splat.data[3].x].Sample(anisoWrap, diffuseUV).rgb * weight.a;
        
        blendedNormal += (diffuseMap[splat.data[0].y].Sample(anisoWrap, diffuseUV).xyz * 2.0 - 1.0) * weight.r;
        blendedNormal += (diffuseMap[splat.data[1].y].Sample(anisoWrap, diffuseUV).xyz * 2.0 - 1.0) * weight.g;
        blendedNormal += (diffuseMap[splat.data[2].y].Sample(anisoWrap, diffuseUV).xyz * 2.0 - 1.0) * weight.b;
        blendedNormal += (diffuseMap[splat.data[3].y].Sample(anisoWrap, diffuseUV).xyz * 2.0 - 1.0) * weight.a;
        
        blendedMetallic += splat.data[0].z * weight.r;
        blendedMetallic += splat.data[1].z * weight.g;
        blendedMetallic += splat.data[2].z * weight.b;
        blendedMetallic += splat.data[3].z * weight.a;
        
        blendedSmoothness += splat.data[0].w * weight.r;
        blendedSmoothness += splat.data[1].w * weight.g;
        blendedSmoothness += splat.data[2].w * weight.b;
        blendedSmoothness += splat.data[3].w * weight.a;
        
        totalWeight += weight.r + weight.g + weight.b + weight.a;

    }
    
    if (totalWeight > 0.0)
    {
        color.rgb /= totalWeight;
        blendedNormal /= totalWeight;
        blendedMetallic /= totalWeight;
        blendedSmoothness /= totalWeight;
    }
    else
    {
        blendedNormal = float3(0, 0, 1);
    }
    blendedNormal = normalize(blendedNormal);
    
    normal = normalize(mul(blendedNormal, float3x3(tangentWS, bitangentWS, normalWS)));
    
    float shadowFactor = CalcShadowFactor(input.ShadowPosH);
    
    output.albedo = color;
    output.normalWS = float4(normal, 0.f);
    output.positionWS = float4(worldPosition, 0.f);
    output.emissive = float4(0.f, 0.f, 0.f, shadowFactor);
    
    return output;
}