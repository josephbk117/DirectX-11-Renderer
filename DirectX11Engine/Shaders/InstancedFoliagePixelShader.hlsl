#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

Texture2D tex : register(t0);
Texture2D tex1 : register(t1);
Texture2D tex2 : register(t2);
Texture2D tex3 : register(t3);
SamplerState splr;

cbuffer CBuf
{
    matrix model;
    matrix view;
    matrix projection;
    matrix modelView;
    matrix modelViewProj;
    float4 camPos;
};

static const float3 materialColour =
{ 0.7f, 0.7f, 0.7f };
static const float3 ambient =
{ 0.05f, 0.05f, 0.05f };
static const float3 diffuseColour =
{ 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.001f;
static const float attQuad = 0.0075f;
static const float specularIntensity = 100.0f;
static const float specularPower = 1.0f;

struct PS_IN
{
    float2 tc : TexCoord;
    
    float3 viewNorm : Normal;
    float3 viewTan : Tangent;
    float3 viewBiTan : BiTangent;
    float3 viewPos : Position;
    
    float3 tPos : Position1;
    float3 tViewPos : Position2;
    float3 lightPos : Position3;
};


float4 main(PS_IN psIn) : SV_Target
{
    float4 outCol = tex.Sample(splr, psIn.tc);
    
    if(outCol.a < 0.5f)
    {
        discard;
    }
    return outCol;
}