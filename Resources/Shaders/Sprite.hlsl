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

float4 PS_Sprite(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;
    float4 texColor = {1.f,1.f,1.f,1.f};
    
    CBUIData uiData = UIData[idx0];
    float4 color = uiData.color;
    texColor = diffuseMap[uiData.textureIdx].SampleLevel(linearClamp, uv, 0);
    texColor.rgb = GammaDecoding(texColor.rgb);
    float4 finalColor = texColor * color;
    float4 color2 = finalColor * 0.5f;
    color2.a = finalColor.a;
    if (uiData.intData0 == 1)
    {
        float fillAmount = uiData.floatData0;
        float fillAmount2 = uiData.floatData1;
        if (uv.x > fillAmount2)
            discard;
        else if (fillAmount < uv.x && uv.x <= fillAmount2)
            return float4(1.f, 1.f, 1.f, finalColor.a * 0.4f);
        else if (fillAmount2 <= uv.x && uv.x <= fillAmount)
            return float4(1.f, 1.f, 1.f, finalColor.a);
        
        //float time = uiData.floatData2;
        //float wave = sin(uv.x * 20 + time * 5) * 0.5;
        //float newY = uv.y + wave;
        
        float t = saturate(uv.x / fillAmount);
        float4 color = lerp(finalColor, color2, t);
        
        return color;
    }
 
    //finalColor.rgb = ToneMapping(finalColor.rgb);
    //finalColor.rgb = GammaEncoding(finalColor.rgb);
    return finalColor;
}