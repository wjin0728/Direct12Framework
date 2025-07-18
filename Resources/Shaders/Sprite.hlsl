#include"Paramiters.hlsl"
#include"Utility.hlsl"

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUTPUT VS_Sprite(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    CBUIData uiData = UIData[idx0];
    float2 uvOffset = uiData.uvOffset;
    float2 uvScale = uiData.uvScale;
    float2 uv = input.uv * uvScale + uvOffset;
    float2 pos = uiData.pos.xy;
    float2 size = uiData.size / renderTargetSize;
    size *= uiData.sizeScale;
    
    output.pos = float4(input.pos.x * size.x + pos.x, input.pos.y * size.y + pos.y, 0.f, 1.f);
    //output.pos = float4(input.pos, 1.f);
    output.uv = input.uv;

    return output;
}

//#define TRANSPARENT_CLIP
float4 PS_Sprite(VS_OUTPUT input) : SV_Target
{
    float4 texColor = {1.f,1.f,1.f,1.f};
    
    CBUIData uiData = UIData[idx0];
    float4 color = uiData.color;
    texColor = diffuseMap[uiData.textureIdx].SampleLevel(linearClamp, input.uv, 0);
    texColor.rgb = GammaDecoding(texColor.rgb);
    float4 finalColor = texColor * color;
    if (uiData.intData0 == 1)
    {
        if (input.uv.x > uiData.floatData1)
            discard;
        else if (uiData.floatData0 < input.uv.x && input.uv.x <= uiData.floatData1)
            return float4(1.f, 1.f, 1.f, finalColor.a * 0.3f);
        else if (uiData.floatData1 <= input.uv.x && input.uv.x <= uiData.floatData0)
            return float4(finalColor.rgb / 3.f, finalColor.a);
    }
 
    //finalColor.rgb = ToneMapping(finalColor.rgb);
    //finalColor.rgb = GammaEncoding(finalColor.rgb);
    return finalColor;
}