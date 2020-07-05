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

float2 ParallaxMapping(float2 texCoords, float3 viewDir)
{
    // number of depth layers
    const float minLayers = 16;
    const float maxLayers = 32;
    float numLayers = lerp(maxLayers, minLayers, abs(dot(float3(0.0, 0.0, 1.0), viewDir)));

    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    
    const float strength = 0.06f;
    
    float2 P = viewDir.xy * strength;
    float2 deltaTexCoords = P / numLayers;
    deltaTexCoords.y *= -1;
    
    // get initial values
    float2 currentTexCoords = texCoords;
    float currentDepthMapValue = tex3.Sample(splr, currentTexCoords).r;
  
    const int loopCount = maxLayers;
    int curLoopCount = 0;
    
    while (curLoopCount < loopCount)
    {
        if (currentLayerDepth >= currentDepthMapValue)
        {
            curLoopCount = loopCount;
        }

        curLoopCount += 1;

        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = tex3.Sample(splr, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = tex3.Sample(splr, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

float4 main(PS_IN psIn) : SV_Target
{
    
     // offset texture coordinates with Parallax Mapping
    float3 viewDir = normalize(psIn.tViewPos - psIn.tPos);
    float2 texCoords = psIn.tc;
    
    texCoords = ParallaxMapping(psIn.tc, viewDir);

    // obtain normal from normal map
    float3 normal = tex2.Sample(splr, texCoords);
    normal = normalize(normal * 2.0 - 1.0);
   
    // get diffuse color
    float3 color = tex.Sample(splr, texCoords);
    // ambient
    float3 ambient = 0.1 * color;
    // diffuse
    
    const LightVectorData lv = CalculateLightVectorData(psIn.lightPos, psIn.tPos); 
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    
    float diff = max(dot(lv.dirToL, normal), 0.0);
    float3 diffuse = diff * att * color;
    // specular    
    float3 reflectDir = reflect(-lv.dirToL, normal);
    float3 halfwayDir = normalize(lv.dirToL + viewDir);
    
    const float4 specularSample = tex1.Sample(splr, texCoords);
    float3 specularReflectionColor = specularSample.rgb;

    float specularPower = pow(2.0f, specularSample.a * 13.0f);
    
    float spec = pow(max(dot(normal, halfwayDir), 0.0), specularPower) * att;

    float3 specular = specularReflectionColor * spec;
    return float4(ambient + diffuse + specular, 1.0);
}