#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

Texture2D tex : register(t0);
Texture2D tex1 : register(t1);
Texture2D tex2 : register(t2);
SamplerState splr;

cbuffer CBuf
{
    matrix view;
    matrix projection;
    matrix modelView;
    matrix modelViewProj;
};

static const float3 materialColour =
{ 0.7f, 0.7f, 0.7f };
static const float3 ambient =
{ 0.05f, 0.05f, 0.05f };
static const float3 diffuseColour =
{ 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.045f;
static const float attQuad = 0.0075f;
static const float specularIntensity = 100.0f;
static const float specularPower = 1.0f;

float4 main(float2 tc : TexCoord, float3 viewNorm : Normal, float3 viewTan : Tangent, float3 viewBiTan : BiTangent, float3 viewPos : Position) : SV_Target
{
    float4 diffuseTex = tex.Sample(splr, tc);
    clip(diffuseTex.a < 0.5f ? -1 : 1);
    
    viewNorm = MapNormal(viewTan, viewBiTan, viewNorm, tc, tex2, splr);
    const LightVectorData lv = CalculateLightVectorData((float3) mul(float4(4, 12, 4, 1.0f), view), viewPos);
        
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    
	// diffuse light
    const float3 diffuse = Diffuse(diffuseColour, diffuseIntensity, att, lv.vToL, viewNorm);

    float3 specularReflectionColor;

    const float4 specularSample = tex1.Sample(splr, tc);
    specularReflectionColor = specularSample.rgb;

    float specularPower = pow(2.0f, specularSample.a * 13.0f);
    
    // specular reflected
    const float3 specularReflected = Speculate(specularReflectionColor, specularIntensity, viewNorm, lv.vToL, viewPos, att, specularPower);
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4( saturate((diffuse + ambient) * diffuseTex.rgb + specularReflected), diffuseTex.a);

}