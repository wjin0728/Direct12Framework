#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b3)
{
    float3 _MainLightColor;
    float offset;
    float3 bottomColor;
    float distance;
    
    float3 padding;
    float falloff;
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
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 positionWS : TEXCOORD0;
    float3 normalWS : TEXCOORD1;
    
    float2 uv : TEXCOORD1;
};

//¡§¡° ºŒ¿Ã¥ı
VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(input.normal, input.tangent);
    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    
    output.normalWS = normalInputs.normalWS;
    
    output.uv = input.uv;
    
    return output;
}


//#define TRANSPARENT_CLIP

//«»ºø ºŒ¿Ã¥ı
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 0.f, 0.f, 1.f);
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    
    float ase_lightIntensity = max(max(_MainLightColor.r, _MainLightColor.g), _MainLightColor.b);
    float4 ase_lightColor = float4(_MainLightColor.rgb / ase_lightIntensity, ase_lightIntensity);
    float dotResult38 = dot(_MainLightPosition.xyz, worldNormal);
    float clampResult104 = clamp((exp2((dotResult38 * _lightingContrast)) * _DirectLight), _lightMin, _lightMax);
				
    float4 appendResult53 = (float4((1.0 - WorldViewDirection.x), (1.0 - WorldViewDirection.y), (1.0 - WorldViewDirection.z), 0.0));
    float dotResult50 = dot(float4(_MainLightPosition.xyz, 0.0), appendResult53);
    float grayscale93 = Luminance(appendResult53.xyz);
    float grayscale98 = Luminance(_MainLightPosition.xyz);
    float clampResult96 = clamp(max((dotResult50 * _lightDirMulti * grayscale93), (_minEmit + (grayscale98 * _minEmit_dir))), 0.0, _maxEmit);
    
    float3 staticSwitch20 = lerpResult18;

    return float4(staticSwitch20, 1.f);
}
