#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float4 _BaseColor;
    float4 _FoamColor;
    float4 _FoamEmitColour;
    float4 _VeryDeepColour;
    float _GlowDepth;
    float _GlowFalloff;
    float _FoamGlowMultiplier;
    float _WaveSpeed;
    float _Specular;
    float _Smoothness;
    float _FoamSmoothness;
    float _ReflectionPower;
    float _OpacityFalloff;
    float _NormalScale;
    float _NormalTiling2;
    float _WaveFoamOpacity;
    float _RippleSpeed;
    float _WaveNoiseScale;
    float _WaveNoiseAmount;
    float _WaveDirection;
    float _WaveWavelength;
    float _WaveAmplitude;
    float _Depth;
    float _OverallFalloff;
    float _ShallowFalloff;
    float _DistortionSpeed;
    float _DistortionTiling;
    float _Distortion;
    float _FoamSpread;
    float _FoamShoreline;
    float _FoamFalloff;
    float _OpacityMin;
    float _NormalTiling;
    float _Opacity;
    int _WaveMask;
    int _FoamMaskIdx;
    int _RipplesNormalIdx;
    int _RipplesNormal2Idx;
    float2 padding;
};

float3 mod2D289(float3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
float2 mod2D289(float2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
float3 permute(float3 x)
{
    return mod2D289(((x * 34.0) + 1.0) * x);
}
float snoise(float2 v)
{
    const float4 C = float4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);
    float2 i1;
    i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod2D289(i);
    float3 p = permute(permute(i.y + float3(0.0, i1.y, 1.0)) + i.x + float3(0.0, i1.x, 1.0));
    float3 m = max(0.5 - float3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
    m = m * m;
    m = m * m;
    float3 x = 2.0 * frac(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);
    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}
			
float4 ComputeGrabScreenPos(float4 pos)
{
    float scale = 1.f; 

    float4 o = pos;
    o.y = pos.w * 0.5f;
    o.y = (pos.y - o.y) * scale + o.y;

    return o;
}
			
float3 mod3D289(float3 x)
{
    return x - floor(x / 289.0) * 289.0;
}
float4 mod3D289(float4 x)
{
    return x - floor(x / 289.0) * 289.0;
}
float4 permute(float4 x)
{
    return mod3D289((x * 34.0 + 1.0) * x);
}
float4 taylorInvSqrt(float4 r)
{
    return 1.79284291400159 - r * 0.85373472095314;
}
float snoise(float3 v)
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
    float4 j = p - 49.0 * floor(p / 49.0); // mod(p,7*7)
    float4 x_ = floor(j / 7.0);
    float4 y_ = floor(j - 7.0 * x_); // mod(j,N)
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
			
float2 voronoihash110(float2 p)
{
						
    p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    return frac(sin(p) * 43758.5453);
}
			
float voronoi110(float2 v, float time, inout float2 id, inout float2 mr, float smoothness, inout float2 smoothId)
{
    float2 n = floor(v);
    float2 f = frac(v);
    float F1 = 8.0;
    float F2 = 8.0;
    float2 mg = 0;
    for (int j = -2; j <= 2; j++)
    {
        for (int i = -2; i <= 2; i++)
        {
            float2 g = float2(i, j);
            float2 o = voronoihash110(n + g);
            o = (sin(time + o * 6.2831) * 0.5 + 0.5);
            float2 r = f - g - o;
            float d = 0.5 * dot(r, r);
            if (d < F1)
            {
                F2 = F1;
                F1 = d;
                mg = g;
                mr = r;
                id = o;
            }
            else if (d < F2)
            {
                F2 = d;
						
            }
        }
    }
    return (F2 + F1) * 0.5;
}


//
//Forward
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 positionWS : TEXCOORD0;
    float3 normalWS : TEXCOORD1;
    float3 tangentWS : TEXCOORD2;
    float3 bitangentWS : TEXCOORD3;
    float4 ShadowPosH : TEXCOORD4;
    
    float4 color : COLOR;
    float2 uv : TEXCOORD5;
};

//정점 셰이더
VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float3 ase_worldPos = mul(float4(input.position, 1.0f), worldMat);
    
    float2 temp_cast_0 = (_WaveSpeed).xx;
    float mulTime307 = deltaTime * 0.001;
    float2 temp_cast_1 = (mulTime307).xx;
    float2 texCoord312 = input.uv.xy * float2(1, 1) + temp_cast_1;
    float simplePerlin2D320 = snoise(texCoord312 * _WaveNoiseScale);
    float2 appendResult59 = (float2(ase_worldPos.x, ase_worldPos.z));
    float cos302 = cos(_WaveDirection);
    float sin302 = sin(_WaveDirection);
    float2 rotator302 = mul(((simplePerlin2D320 * _WaveNoiseAmount) + appendResult59) - float2(0, 0), float2x2(cos302, -sin302, sin302, cos302)) + float2(0, 0);
    float2 temp_output_60_0 = (rotator302 * _WaveWavelength);
    float2 panner127 = (1.0 * _Time.y * temp_cast_0 + temp_output_60_0);
    float4 temp_cast_2 = 0;
    float4 lerpResult149 = lerp(diffuseMap[_WaveMask].SampleLevel(linearWrap, panner127, 0), temp_cast_2, (1.0 - _WaveAmplitude));
    float4 waveCrestVertoffset153 = lerpResult149;
    float grayscale298 = Luminance(waveCrestVertoffset153.rgb);
    float4 appendResult301 = (float4(0.0, grayscale298, 0.0, 0.0));
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(appendResult301.xyz);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(input.normal, input.tangent);
    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    
    output.normalWS = normalInputs.normalWS;
    output.tangentWS = normalInputs.tangentWS;
    output.bitangentWS = normalInputs.bitangentWS;
    
    output.ShadowPosH = mul(output.positionWS, shadowTransform);
        
    output.color = input.color;
    output.uv = input.uv;
    
    return output;
}


#define TRANSPARENT_CLIP

//픽셀 셰이더
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = input.tangentWS;
    float3 worldBitangent = input.bitangentWS;
    float2 uv = input.uv;   

    // Shoreline foam
    float foam62 = saturate(pow(distanceDepth170 + _FoamShoreline, _FoamFalloff));

// Perlin 기반의 얕은 수면 거품
    float2 panner166 = (0.1 * totalTime * float2(1, 0) + worldPosition.xz);
    float2 panner22 = (0.1 * totalTime * float2(-1, 0) + worldPosition.xz);
    float noise1 = snoise(float3(panner166 * 1.5, 0.0));
    float noise2 = snoise(float3(panner22 * 3.0, 0.0));
    float foamNoiseMask = 1.0 - step((noise1 + noise2), (distanceDepth170 * _FoamSpread));
    float4 foamColorCombined = saturate((_FoamColor * foamNoiseMask) + (_FoamColor * foam62));

// 거품을 포함한 색상
    float4 colorWithFoam = lerp(_BaseColor, float4(1, 1, 1, 0), foamColorCombined);

// 파도 꼭대기 거품
    float2 panner70 = (1.0 * totalTime * _WaveSpeed.xx + (worldPosition.xz * _WaveWavelength));
    float crestNoise1 = snoise(float3((worldPosition.xz + 0.1 * totalTime * float2(1, 0)) * 2, 0.0));
    float crestNoise2 = snoise(float3((worldPosition.xz + 0.1 * totalTime * float2(-1, 0)) * 0.8, 0.0));
    float crestFoamMask = step((crestNoise1 + crestNoise2), 0.0);
    float4 crestFoamTex = diffuseMap[_FoamMaskIdx].SampleLevel(linearWrap, panner70, 0);
    float4 crestFoamColor = lerp(float4(0, 0, 0, 0), crestFoamTex, crestFoamMask);
    float4 waveCrestFoam = lerp(float4(0, 0, 0, 0), crestFoamColor, _WaveFoamOpacity);
    
    float specular = lerp(_Specular, 0.0, foam62);
    float smoothness = lerp(_Smoothness, _FoamSmoothness, foam62) * _ReflectionPower;

// 최종 Albedo에 파도 거품 추가
    float4 waterAlbedo = colorWithFoam + waveCrestFoam;
    
    #ifdef TRANSPARENT_CLIP
    clip(color.a - 0.5);
    #endif
    
    
    
    float3 camDir = (camPos - worldPosition);
    float distToEye = length(camDir);
    camDir /= distToEye;

    LightingData lightingData = (LightingData) 0;
    lightingData.cameraDirection = camDir;
    lightingData.normalWS = normal;
    lightingData.positionWS = worldPosition;
    lightingData.shadowFactor = CalcShadowFactor(input.ShadowPosH);
    
    SurfaceData surfaceData = (SurfaceData) 0;
    surfaceData.albedo = waterAlbedo.rgb;
    surfaceData.metallic = 0.f;
    surfaceData.smoothness = smoothness;
    surfaceData.specular = 0.5f;
    surfaceData.emissive = 0.f;
    
#ifdef LIGHTING
    float3 finalColor = CalculatePhongLight(lightingData, surfaceData);
#else
    float3 finalColor = color.rgb;
#endif
    
    color.xyz = GammaEncoding(finalColor);
    
#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
    color = lerp(color, gFogColor, fogAmount);
#endif

    return float4(color.rgb, 1.f);
}

//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Shadow Cast
struct VS_SHADOW_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VS_SHADOW_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

VS_SHADOW_OUTPUT VS_Shadow(VS_SHADOW_INPUT input)
{
    VS_SHADOW_OUTPUT output = (VS_SHADOW_OUTPUT) 0;
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
    
    output.position = positionInputs.positionCS;
    output.uv = input.uv;
    output.color = input.color;
    
    return output;
}

void PS_Shadow(VS_SHADOW_OUTPUT input)
{
    float2 uvLeaf = input.uv;
    float2 uvTrunk = input.uv;
    
    float4 texColor = (input.color.b > 0.5) ? diffuseMap[leafTexIdx].Sample(anisoClamp, uvLeaf) : diffuseMap[trunkTexIdx].Sample(anisoClamp, uvTrunk);
    float4 color = max(texColor, float4(leafColor, 0.f));
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.5);
#endif
}


//
//G Pass
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

VS_OUTPUT VS_GPass(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position);
    VertexNormalInputs normalInputs = GetVertexNormalInputs(input.normal, input.tangent);
    
    output.positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    
    output.normalWS = normalInputs.normalWS;
    output.tangentWS = normalInputs.tangentWS;
    output.bitangentWS = normalInputs.bitangentWS;
    
    output.ShadowPosH = mul(output.positionWS, shadowTransform);
        
    output.color = input.color;
    output.uv = input.uv;
    
    return output;
}

struct PS_GPASS_OUTPUT
{
    float4 albedo : SV_Target0;
    float4 normalWS : SV_Target1;
    float4 emissive : SV_Target2;
    float4 positionWS : SV_Target3;
};

PS_GPASS_OUTPUT PS_GPass(VS_OUTPUT input) : SV_Target
{
    PS_GPASS_OUTPUT output = (PS_GPASS_OUTPUT) 0;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float3 worldPosition = input.positionWS.xyz;
    float3 worldNormal = normalize(input.normalWS);
    float3 normal = worldNormal;
    float3 worldTangent = input.tangentWS;
    float3 worldBitangent = input.bitangentWS;
    float2 uvLeaf = input.uv;
    float2 uvTrunk = input.uv;

    float4 texColor = (input.color.b > 0.5) ? diffuseMap[leafTexIdx].Sample(anisoClamp, uvLeaf) : diffuseMap[trunkTexIdx].Sample(anisoClamp, uvTrunk);
    
    color = max(texColor, float4(0.f, 0.f, 0.f, 0.f));
    
#ifdef TRANSPARENT_CLIP
    clip(color.a - 0.5);
#endif
    
    if (input.color.b > 0.5 && leafNormalIdx != -1)
    {
        float3 normalMapSample = diffuseMap[leafNormalIdx].Sample(anisoClamp, uvLeaf).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    else if (input.color.b <= 0.5 && trunkNormalIdx != -1)
    {
        float3 normalMapSample = diffuseMap[trunkNormalIdx].Sample(anisoClamp, uvTrunk).rgb;
        normal = NormalSampleToWorldSpace(normalMapSample, worldNormal, worldTangent, worldBitangent);
    }
    float temp = (1.0 - input.color.b);
    float lerpResult188 = lerp(leafMetallic, 0.0, temp);
    float lerpResult195 = lerp(trunkMetallic, 0.0, input.color.b);
				
    float lerpResult191 = lerp(leafSmoothness, 0.0, temp);
    float lerpResult190 = lerp(trunkSmoothness, 0.0, input.color.b);
    
    float shadowFactor = CalcShadowFactor(input.ShadowPosH);
    
    output.albedo = color;
    output.normalWS = float4(normal, lerpResult188 + lerpResult195);
    output.positionWS = float4(worldPosition, lerpResult191 + lerpResult190);
    output.emissive = float4(0.f, 0.f, 0.f, shadowFactor);
    
    return output;
}