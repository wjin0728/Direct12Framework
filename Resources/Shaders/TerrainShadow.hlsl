#include"Paramiters.hlsl"
#include"Utility.hlsl"

#define DYNAMIC_TESSELLATION

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
};


//¡§¡° ºŒ¿Ã¥ı∏¶ ¡§¿««—¥Ÿ.
VS_INPUT VS_Main(VS_INPUT input)
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
};

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HS_OUTPUT HS_Main(InputPatch<VS_INPUT, 4> p, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HS_OUTPUT hout;
	
    hout.PosL = p[i].position;
	
    return hout;
}

struct DS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
};

[domain("quad")]
DS_OUTPUT DS_Main(TessFactor tessFactors, float2 uv : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 4> quad)
{
    DS_OUTPUT dout;
	
    float3 v1 = lerp(quad[0].PosL, quad[1].PosL, uv.x);
    float3 v2 = lerp(quad[2].PosL, quad[3].PosL, uv.x);
    float3 p = lerp(v1, v2, uv.y);
	
    Texture2D heightMap = diffuseMap[terrainData.heightMapIdx];
    p.y = heightMap.SampleLevel(linearWrap, dout.uv, 0).r*255.f * terrainData.scale.y;
    
    dout.worldPos = float4(p, 1.0f);
    dout.position = mul(dout.worldPos, viewProjMat);
	
    return dout;
}

#define TRANSPARENT_CLIP

//«»ºø ºŒ¿Ã¥ı
void PS_Main(DS_OUTPUT input)
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    Material mat = terrainData.material;
    
    float4 texColor = diffuseMap[mat.diffuseMapIdx].Sample(linearWrap, input.uv);
    color = float4(GammaDecoding(texColor.rgb), texColor.a);
  
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.1);
#endif
    #ifdef FOG
    float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
    color = lerp(color, float4(0.7f, 0.7f, 0.7f, 1.0f), fogAmount);
    #endif
}