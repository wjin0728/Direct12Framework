#define DIRECTIONAL_LIGHT 10
#define POINT_LIGHT 10
#define SPOT_LIGHT 10
#define MAX_MATERIAL 10


struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
};


struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};

//«»ºø ºŒ¿Ã¥ı∏¶ ¡§¿««—¥Ÿ.
float4 main(VS_OUTPUT input) : SV_TARGET
{
   
    return float4(1.f,0.f,0.f,1.f);
}