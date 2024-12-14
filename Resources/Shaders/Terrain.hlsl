#include"Paramiters.hlsl"
#include"Utility.hlsl"


struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float2 uv : TEXCOORD;
};



//¡§¡° ºŒ¿Ã¥ı∏¶ ¡§¿««—¥Ÿ.
VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.worldPos = float4(input.position, 1.0f);
    output.position = mul(output.worldPos, viewProjMat);
    output.worldNormal = input.normal;
    output.worldTangent = input.tangent;
    output.uv = input.uv;
    
    return output;
}

struct TessFactor
{
    float EdgeTess[4] : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

TessFactor ConstantHS(InputPatch<VS_OUTPUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    TessFactor pt;
	
    float3 centerL = 0.25f * (patch[0].position + patch[1].position + patch[2].position + patch[3].position);
    float3 centerW = mul(float4(centerL, 1.0f), wor).xyz;
	
    float d = distance(centerW, gEyePosW);

	// Tessellate the patch based on distance from the eye such that
	// the tessellation is 0 if d >= d1 and 64 if d <= d0.  The interval
	// [d0, d1] defines the range we tessellate in.
	
    const float d0 = 20.0f;
    const float d1 = 100.0f;
    float tess = 64.0f * saturate((d1 - d) / (d1 - d0));

	// Uniformly tessellate the patch.

    pt.EdgeTess[0] = tess;
    pt.EdgeTess[1] = tess;
    pt.EdgeTess[2] = tess;
    pt.EdgeTess[3] = tess;
	
    pt.InsideTess[0] = tess;
    pt.InsideTess[1] = tess;
	
    return pt;
}

struct HS_OUTPUT
{
    float3 PosL : POSITION;
};

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HS_OUTPUT HS_Main(InputPatch<VS_OUTPUT, 4> p, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HS_OUTPUT hout;
	
    hout.PosL = p[i].position;
	
    return hout;
}

struct DS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float2 uv : TEXCOORD;
};

[domain("quad")]
DS_OUTPUT DS_Main(TessFactor tessFactors, float2 uv : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 4> quad)
{
    DS_OUTPUT dout;
	
    float3 v1 = lerp(quad[0].PosL, quad[1].PosL, uv.x);
    float3 v2 = lerp(quad[2].PosL, quad[3].PosL, uv.x);
    float3 p = lerp(v1, v2, uv.y);
	
    p.y = diffuseMap[terrainMat.heightMapIdx].Sample(linearClamp, uv);
    
    float4 posW = float4(p, 1.0f);
    dout.PosH = mul(posW, gViewProj);
	
    return dout;
}

#define TRANSPARENT_CLIP

//«»ºø ºŒ¿Ã¥ı
[earlydepthstencil]
float4 PS_Main(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    Material mat = terrainMat.material;
    
    float4 texColor = diffuseMap[mat.diffuseMapIdx].Sample(pointWrap, input.uv);
    color = float4(GammaDecoding(texColor.rgb), texColor.a);
    
    if (terrainMat.detailMapTdx != -1)
    {
        float4 detailColor = diffuseMap[terrainMat.detailMapTdx].Sample(linearWrap, input.uv * 50.f);
   
        color = lerp(color, float4(GammaDecoding(detailColor.rgb), detailColor.a), 0.5);
    }
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.1);
#endif
    
    float3 normal = normalize(input.worldNormal);
    float3 camDir = (camPos - input.worldPos.xyz);
    float distToEye = length(camDir);
    camDir /= distToEye;
    
    LightColor finalColor = CalculatePhongLight(input.position.xyz, normal, camDir, mat);
    
    color.xyz = GammaEncoding((finalColor.diffuse.xyz * color.xyz) + finalColor.specular.xyz + (0.05 * color.xyz));
    
    #ifdef FOG
    float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
    color = lerp(color, float4(0.7f, 0.7f, 0.7f, 1.0f), fogAmount);
#endif

    return float4(color.xyz, 1.f);
}