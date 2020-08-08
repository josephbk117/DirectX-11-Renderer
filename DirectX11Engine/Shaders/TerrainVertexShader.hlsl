cbuffer CBuf
{
    matrix model;
    matrix view;
    matrix projection;
    matrix modelView;
    matrix modelViewProj;
    float4 camPos;
};
cbuffer DetailInfo
{
    float maxTessellationAmount;
    float minDistance;
    float maxDistance;
    int smoothing;
};
struct VS_Out
{
    matrix modelViewProj : MATRIX;
    float tessFactor : TexCoord1;
    float2 tex : TexCoord;
    float3 worldPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 biTan : BiTangent;
    float4 pos : SV_Position;
};

VS_Out main(float3 pos : Position, float2 tex : TexCoord, float3 norm : Normal, float3 tan : Tangent, float3 biTan : BiTangent)
{
    VS_Out vso;
    vso.modelViewProj = modelViewProj;
    vso.worldPos = pos;
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.normal = norm;
    vso.tan = tan;
    vso.biTan = biTan;
    vso.tex = tex;
    float distanceVal = distance(pos, camPos.xyz);
    vso.tessFactor = distanceVal < minDistance ? maxTessellationAmount : (1.0f - min(distanceVal, maxDistance) / maxDistance) * maxTessellationAmount;
    vso.tessFactor = max(vso.tessFactor, 1.0f);
    return vso;
}