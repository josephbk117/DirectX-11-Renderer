Texture2D tex[3] : register(t0);
Texture2D tex1[3] : register(t3);
Texture2D tex2 : register(t6);

SamplerState splr;

cbuffer TerrainInfo
{
    float4 colourBlendInfo[3];
    vector<float, 3> texScale;
    vector<float, 3> startHeight;
    float heightScale;
};

struct PS_In
{
    float2 tex : TexCoord;
    float3 worldPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 biTan : BiTangent;
    float4 pos : SV_Position;
};

float inverseLerp(float a, float b, float value)
{
    return saturate((value - a) / (b - a));
}

float3 triplanar(float3 worldPos, float scale, float3 blendAxes, int texIndex)
{
    float3 scaledworldPos = worldPos * scale;
    
    float3 xproj = tex[texIndex].Sample(splr, scaledworldPos.yz) * blendAxes.x;
    float3 yproj = tex[texIndex].Sample(splr, scaledworldPos.xz) * blendAxes.y;
    float3 zproj = tex[texIndex].Sample(splr, scaledworldPos.xy) * blendAxes.z;
    
    return xproj + yproj + zproj;
}

float3 triplanarNorm(float3 worldPos, float scale, float3 blendAxes, int texIndex)
{
    float3 scaledworldPos = worldPos * scale;
    
    float3 xproj = tex1[texIndex].Sample(splr, scaledworldPos.yz) * blendAxes.x;
    float3 yproj = tex1[texIndex].Sample(splr, scaledworldPos.xz) * blendAxes.y;
    float3 zproj = tex1[texIndex].Sample(splr, scaledworldPos.xy) * blendAxes.z;
    
    return xproj + yproj + zproj;
}

// Reoriented Normal Blending
float3 blend_rnm(float3 n1, float3 n2)
{
    n1 = n1 * 0.5 + 0.5;
    n2 = n2 * 0.5 + 0.5;
    n1 = n1 * float3(2, 2, 2) + float3(-1, -1, 0);
    n2 = n2 * float3(-2, -2, 2) + float3(1, 1, -1);
    return n1 * dot(n1, n2) / n1.z - n2;
}


float4 main(PS_In psIn, uint tid : SV_PrimitiveID) : SV_Target
{
    const float3 texNormal = normalize(psIn.normal); //normalize(tex2.Sample(splr, psIn.tex).rgb);
    const float heightPercent = inverseLerp(0.0f, heightScale * 2.0f, psIn.worldPos.y + heightScale);
    const float e = 1E-4;
    
    float3 blendAxes = abs(texNormal);
    blendAxes /= blendAxes.x + blendAxes.y + blendAxes.z;
    
    float3 layerCol = (0, 0, 0);
    float3 normCol = (0, 0, 0);

    for (int i = 0; i < 3; i++)
    {
        const float drawStr = inverseLerp(-colourBlendInfo[i].a / 2 - e, colourBlendInfo[i].a / 2, heightPercent - startHeight[i]);
        const float3 texCol = triplanar(psIn.worldPos, texScale[i], blendAxes, i);
        const float3 nCol = triplanarNorm(psIn.worldPos, texScale[i], blendAxes, i);

        layerCol = (layerCol * (1.0f - drawStr)) + texCol * colourBlendInfo[i].rgb * drawStr;
        normCol = (normCol * (1.0f - drawStr)) + nCol * drawStr;
    }
    
    layerCol = pow(layerCol, float3((1.0f / 2.2f).rrr));

     // build the tranform (rotation) into same space as tan/bitan/normal (target space)
    const float3x3 tanToTarget = float3x3(normalize(psIn.tan), normalize(psIn.biTan), texNormal);
    const float3 tanNormal = normCol * 2.0f - 1.0f;

    // bring normal from tanspace into target space
    normCol = normalize(mul(tanNormal, tanToTarget));
    const float vdot = max(dot(normalize(float3(0, 1, 1)), normCol), 0.0f);
    return float4((layerCol * vdot) + layerCol * 0.2f, 1.0f);

}