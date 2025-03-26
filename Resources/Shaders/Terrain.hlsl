#include"Paramiters.hlsl"
#include"Utility.hlsl"

//#define DYNAMIC_TESSELLATION

cbuffer MaterialData : register(b0, space1)
{
    float3 size;
    int heightMapIdx;

    int splatNum;
    int alphaMapIdx[TERRAIN_SPLAT_COUNT];
    int diffuseIdx[TERRAIN_SPLAT_COUNT * 4];
    int normalIdx[TERRAIN_SPLAT_COUNT * 4];

    float metallic[TERRAIN_SPLAT_COUNT * 4];
    float smoothness[TERRAIN_SPLAT_COUNT * 4];
};

float CalculateTessFactor(float3 position)
{
    float d = distance(position, camPos);
    
    const float dMin = 100.0f;
    const float dMax = 1000.0f;
    float s = 64.f *saturate((dMax - d) / (dMax - dMin));
    
    return s;
}


struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};


//¡§¡° ºŒ¿Ã¥ı∏¶ ¡§¿««—¥Ÿ.
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
	pt.EdgeTess[0] = 64.f;
    pt.EdgeTess[1] = 64.f;
    pt.EdgeTess[2] = 64.f;
    pt.EdgeTess[3] = 64.f;
    
    pt.InsideTess[0] = 64.f;
    pt.InsideTess[1] = 64.f;
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
	
    Texture2D heightMap = diffuseMap[74];
    p.y = heightMap.SampleLevel(linearWrap, dout.uv, 0).r * 1.f;
    
    dout.worldPos = float4(p, 1.0f);
    dout.position = mul(dout.worldPos, viewProjMat);
	
    return dout;
}

#define TRANSPARENT_CLIP

//«»ºø ºŒ¿Ã¥ı
float4 PS_Forward(DS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    
    float4 texColor = diffuseMap[20].Sample(linearWrap, input.uv);
    color = float4(GammaDecoding(texColor.rgb), texColor.a);
    return texColor;
    
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.1);
#endif
    Texture2D heightMap = diffuseMap[heightMapIdx];
    float4 height = heightMap.SampleLevel(linearClamp, input.uv, 0);
    
    float heightLeft = heightMap.SampleLevel(linearClamp, input.uv - float2(0.01, 0), 0).r;
    float heightRight = heightMap.SampleLevel(linearClamp, input.uv + float2(0.01, 0), 0).r;
    float heightDown = heightMap.SampleLevel(linearClamp, input.uv - float2(0, 0.01), 0).r;
    float heightUp = heightMap.SampleLevel(linearClamp, input.uv + float2(0, 0.01), 0).r;
    
    float3 tangent = float3(1.0, (heightRight - heightLeft), 0.f) * size;
    float3 bitangent = float3(0.0, (heightDown - heightUp), -1.f) * size;
    
    float3 normal = cross(normalize(tangent), normalize(bitangent));
    float3 camDir = (camPos - input.worldPos.xyz);
    float distToEye = length(camDir);
    camDir /= distToEye;
    
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    
    LightingData lightingData = (LightingData) 0;
    lightingData.cameraDirection = camDir;
    lightingData.normalWS = normal;
    lightingData.positionWS = input.worldPos.xyz;
    lightingData.shadowFactor = 1.f;
    
    SurfaceData surfaceData = (SurfaceData) 0;
    surfaceData.albedo = color.rgb;
    surfaceData.metallic = metallic[0];
    surfaceData.smoothness = smoothness[0];
    surfaceData.specular = 0.5f;
    
    float3 finalColor = CalculatePhongLight(lightingData, surfaceData);
    
    color.xyz = GammaEncoding(finalColor);
    
    #ifdef FOG
    float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
    color = lerp(color, gFogColor, fogAmount);
#endif

    return float4(color.xyz, 1.f);
}