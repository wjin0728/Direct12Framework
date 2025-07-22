#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float4 emissiveColor;
    float lightDirMultiplier;
    float minEmit; 
    float minEmitDir; 
    float maxEmit; 
    
    float directLight; 
    float lightMin; 
    float lightingContrast; 
    float lightMax; 
    
    float windEffect; 
    float panningSpeed; 
    float windNoiseScale; 
    float windWorldScale; 
    
    float xMultiplier;
    float yMultiplier;
    float zMultiplier;

    uint noiseTexIdx;
};

//
//Forward
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 positionWS : TEXCOORD0;
    float3 normalWS : TEXCOORD1;
};

inline float3 mod3D289(float3 x)
{
    return x - floor(x / 289.0) * 289.0;
}
inline float4 mod3D289(float4 x)
{
    return x - floor(x / 289.0) * 289.0;
}
inline float4 permute(float4 x)
{
    return mod3D289((x * 34.0 + 1.0) * x);
}
inline float4 taylorInvSqrt(float4 r)
{
    return 1.79284291400159 - r * 0.85373472095314;
}

inline float snoise(float3 v)
{
    const float2 C = float2(1.0 / 6.0, 1.0 / 3.0);
    float3 i = floor(v + dot(v, C.yyy));
    float3 x0 = v - i + dot(i, C.xxx);
    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1.0 - g;
    float3 i1 = min(g.xyz, l.zxy);
    float3 i2 = max(g.xyz, l.zxy);
    float3 x1 = x0 - i1 + C.xxx;
    float3 x2 = x0 - i2 + C.yyy;
    float3 x3 = x0 - 0.5;
    i = mod3D289(i);
    float4 p = permute(permute(permute(i.z + float4(0.0, i1.z, i2.z, 1.0)) + i.y + float4(0.0, i1.y, i2.y, 1.0)) + i.x + float4(0.0, i1.x, i2.x, 1.0));
    float4 j = p - 49.0 * floor(p / 49.0);
    float4 x_ = floor(j / 7.0);
    float4 y_ = floor(j - 7.0 * x_);
    float4 x = (x_ * 2.0 + 0.5) / 7.0 - 1.0;
    float4 y = (y_ * 2.0 + 0.5) / 7.0 - 1.0;
    float4 h = 1.0 - abs(x) - abs(y);
    float4 b0 = float4(x.xy, y.xy);
    float4 b1 = float4(x.zw, y.zw);
    float4 s0 = floor(b0) * 2.0 + 1.0;
    float4 s1 = floor(b1) * 2.0 + 1.0;
    float4 sh = -step(h, 0.0);
    float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;
    float3 g0 = float3(a0.xy, h.x);
    float3 g1 = float3(a0.zw, h.y);
    float3 g2 = float3(a1.xy, h.z);
    float3 g3 = float3(a1.zw, h.w);
    float4 norm = taylorInvSqrt(float4(dot(g0, g0), dot(g1, g1), dot(g2, g2), dot(g3, g3)));
    g0 *= norm.x;
    g1 *= norm.y;
    g2 *= norm.z;
    g3 *= norm.w;
    float4 m = max(0.6 - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
    m = m * m;
    m = m * m;
    float4 px = float4(dot(x0, g0), dot(x1, g1), dot(x2, g2), dot(x3, g3));
    return 42.0 * dot(m, px);
}

struct PS_GPASS_OUTPUT
{
    float4 albedo : SV_Target0;
    float4 normalWS : SV_Target1;
    float4 emissive : SV_Target2;
    float4 positionWS : SV_Target3;
    float4 depth : SV_Target4;
};

VS_OUTPUT VS_GPass(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    float3 positionOS = input.position;
    float4 positionWS = mul(float4(positionOS, 1.0f), worldMat);
    
    float3 noiseCoord = positionWS.xyz * 0.5f;
    float perlin = snoise(noiseCoord * windNoiseScale);
    perlin = perlin * 0.5f + 0.5f;

    float3 blendNoise = float3(perlin, perlin, perlin);
    float2 pannerUV = (panningSpeed * totalTime) + (positionWS.xy * windWorldScale * blendNoise.xy);
    float4 noiseSample = diffuseMap[noiseTexIdx].SampleLevel(linearWrap, pannerUV, 0);

    float3 displacement = float3(
        xMultiplier * noiseSample.r,
        yMultiplier * noiseSample.g,
        zMultiplier * noiseSample.b
    );
    
    displacement *= windEffect;
    
    positionOS += displacement;
    VertexPositionInputs positionInputs = GetVertexPositionInputs(positionOS);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(input.normal, input.tangent);
    
    
    output.position = positionInputs.positionCS;
    output.positionWS = positionInputs.positionWS;
    output.normalWS = normalInputs.normalWS;

    return output;
}

PS_GPASS_OUTPUT PS_GPass(VS_OUTPUT input)
{
    PS_GPASS_OUTPUT output = (PS_GPASS_OUTPUT) 0;
    CBLightsData lightingData = lights[0]; 
    
    float3 worldPosition = input.positionWS.xyz;
    float3 camDir = (camPos - input.positionWS.xyz);
    float distToEye = length(camDir);
    camDir /= distToEye;
    
    float3 lightDir = -normalize(lightingData.directionWS);
    float3 mainLightColor = lightingData.lColor;
    mainLightColor = GammaDecoding(mainLightColor);
    float3 normal = normalize(input.normalWS);
    
    float lightIntensity = max(max(mainLightColor.r, mainLightColor.g), mainLightColor.b);
    float3 lightColorNormalized = mainLightColor.rgb / max(lightIntensity, 0.001); 
    float3 lightColorFinal = lightColorNormalized * lightIntensity;
    
    float NdotL = dot(normal, lightDir);
    float directLighting = clamp(exp2(NdotL * lightingContrast) * directLight, lightMin, lightMax);
    
    float3 invViewDir = 1.0f - camDir;
    float dotLightView = dot(lightDir, invViewDir);
    
    float viewLum = Luminance(invViewDir); 
    float lightLum = Luminance(lightDir);
    float minEmitTerm = minEmit + (lightLum * minEmitDir);
    float emitStrength = clamp(max(dotLightView * lightDirMultiplier * viewLum, minEmitTerm), 0.0, maxEmit);
    
    
    float3 emissiveResult = GammaDecoding(emissiveColor.rgb) * emitStrength;
    
    output.albedo = float4(lightColorFinal * directLighting, 1.f);
    output.normalWS = float4(normal, 0.f);
    output.emissive = float4(emissiveResult, 0.f);
    output.positionWS = float4(worldPosition, 0.f);
    output.depth = float4(0.f, 0.f, 0.f, input.position.z);
    
    return output;
}