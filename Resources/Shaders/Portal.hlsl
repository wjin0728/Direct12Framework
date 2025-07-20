#include"Paramiters.hlsl"
#include"Utility.hlsl"

cbuffer MaterialData : register(b5)
{
    float4 mainColor; // 0
    
    float _WaveAmplitude; // 1
    float _WaveDirection; // 2
    float _WaveSpeed; // 3
    float _WaveWavelength; // 4
    float _WaveNoiseScale; // 5
    float _WaveNoiseAmount; // 6
    uint mainTextureIndex; // 6
    uint _WaveMask; // 7
};

inline float3 mod2D289(float3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
inline float2 mod2D289(float2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
inline float3 permute(float3 x)
{
    return mod2D289(((x * 34.0) + 1.0) * x);
}
inline float snoise(float2 v)
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
    float3 positionWS : POSITION;
    float4 positionCS : TEXCOORD0;
    float2 uv : TEXCOORD5;
};

//Á¤Á¡ ¼ÎÀÌ´õ
VS_OUTPUT VS_Forward(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float3 worldPos = mul(float4(input.position, 1.0f), worldMat).xyz;
    
    float2 temp_cast_0 = (_WaveSpeed).xx;
    float mulTime307 = totalTime * 0.001;
    float2 temp_cast_1 = (mulTime307).xx;
    float2 texCoord312 = input.uv.xy * float2(1, 1) + temp_cast_1;
    float simplePerlin2D320 = snoise(texCoord312 * _WaveNoiseScale);
    float2 appendResult59 = (float2(worldPos.x, worldPos.z));
    float cos302 = cos(_WaveDirection);
    float sin302 = sin(_WaveDirection);
    float2 rotator302 = mul(((simplePerlin2D320 * _WaveNoiseAmount) + appendResult59) - float2(0, 0), float2x2(cos302, -sin302, sin302, cos302)) + float2(0, 0);
    float2 temp_output_60_0 = (rotator302 * _WaveWavelength);
    float2 panner127 = (totalTime * temp_cast_0 + temp_output_60_0);
    float4 temp_cast_2 = 0;
    float4 lerpResult149 = lerp(diffuseMap[_WaveMask].SampleLevel(linearWrap, panner127, 0).rrrr, temp_cast_2, (1.0 - _WaveAmplitude));
    float4 waveCrestVertoffset153 = lerpResult149;
    float grayscale298 = Luminance(waveCrestVertoffset153.rgb);
    float4 appendResult301 = (float4(0.0, grayscale298, 0.0, 0.0));
    
    VertexPositionInputs positionInputs = GetVertexPositionInputs(input.position + appendResult301.xyz);
    
    float4 positionWS = positionInputs.positionWS;
    output.position = positionInputs.positionCS;
    output.positionCS = positionInputs.positionCS;
    output.positionWS = positionWS;
    
    //uv transformation
    float2 uv = mul(float3(input.uv, 1.0), (float3x3) texMat).xy;
    
    output.uv = input.uv;
    
    return output;
}


//ÇÈ¼¿ ¼ÎÀÌ´õ
float4 PS_Forward(VS_OUTPUT input) : SV_TARGET
{
    float4 color = mainColor;
    float3 worldPosition = input.positionWS;
    float2 uv = input.uv;   
    
    
    float4 clipPos = input.positionCS;
    float4 screenPos = ComputeScreenPos(clipPos);
    float2 screenUV = input.position.xy / renderTargetSize;
    
    float sceneDepth = GetNormalizedSceneDepth(screenUV);
    float linearSceneDepth = GetCameraDepth(sceneDepth);
    float linearFragmentDepth = GetCameraDepth(screenPos.z);
    
    
    return float4(finalColor, waterOpacity);
}

//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Shadow Cast


//
//G Pass
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
