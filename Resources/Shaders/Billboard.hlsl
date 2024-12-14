#include"Paramiters.hlsl"
#include"Utility.hlsl"

struct VS_INPUT
{
    //정점 데이터
    float3 position : POSITION;
    
    //인스턴스 데이터
    float3 positionW : POSITIONW;
    float2 size : SIZE;
    uint matIdx : MATERIAL_IDX;
    float4x4 texMat : texMatrix;
};

struct VS_OUTPUT
{
    float3 center : POSITION;
    float2 size : SIZE;
    uint matIdx : MATERIAL_IDX;
    float4x4 texMat : texMatrix;
};

struct GS_OUTPUT
{
    float4 posH : SV_POSITION;
    float3 posW : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    uint matIdx : MATERIAL_IDX;
};


VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.center = input.positionW;
    output.matIdx = input.matIdx;
    output.size = input.size;
    output.texMat = input.texMat;
    
    return output;
}

[maxvertexcount(4)]
void GS_Main(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> triStream)
{
    float3 up = float3(0.0f, 1.0f, 0.0f);
    
    float3 look = camPos - input[0].center;
    look.y = 0.0f; 
    look = normalize(look);
    float3 right = cross(up, look);
    
    float halfWidth = input[0].size.x;
    float halfHeight = input[0].size.y;
	
    float4 v[4];
    v[0] = float4(input[0].center + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].center + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].center - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].center - halfWidth * right + halfHeight * up, 1.0f);
	
    float2 texC[4] =
    {
        mul(float4(float2(0.0f, 1.0f), 0.f, 1.f), input[0].texMat).xy,
		 mul(float4(float2(0.0f, 0.0f), 0.f, 1.f), input[0].texMat).xy,
		 mul(float4(float2(1.0f, 1.0f), 0.f, 1.f), input[0].texMat).xy,
		 mul(float4(float2(1.0f, 0.0f), 0.f, 1.f), input[0].texMat).xy,
    };
    
    
    GS_OUTPUT gout;
	[unroll]
    for (int i = 0; i < 4; ++i)
    {
        gout.posH = mul(v[i], viewProjMat);
        gout.posW = v[i].xyz;
        gout.normal = look;
        gout.texCoord = texC[i];
        gout.matIdx = input[0].matIdx;
		
        triStream.Append(gout);
    }
}

#define TRANSPARENT_CLIP

float4 PS_Main(GS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    Material material = materials[input.matIdx];
    int diffuseMapIdx = material.diffuseMapIdx;
    int normalMapIdx = material.normalMapIdx;
    
    float4 texColor = diffuseMap[diffuseMapIdx].Sample(anisoWrap, input.texCoord);
    color = float4(GammaDecoding(texColor.rgb), texColor.a);
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.05f);
#endif
    
    float3 normal = normalize(input.normal);
    float3 camDir = (camPos - input.posW.xyz);
    float distToEye = length(camDir);
    camDir /= distToEye;
    
    
    LightColor finalColor = CalculatePhongLight(input.posW.xyz, normal, camDir, material);
    
    color.xyz = GammaEncoding((finalColor.diffuse.xyz * color.xyz) + finalColor.specular.xyz + (0.1 * color.xyz));
    
   #ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
    color = lerp(color, gFogColor, fogAmount);
#endif

    return float4(color.xyz, 1.f);
}