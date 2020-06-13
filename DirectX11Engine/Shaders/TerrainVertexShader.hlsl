cbuffer CBuf
{
    matrix model;
    matrix view;
    matrix projection;
    matrix modelView;
    matrix modelViewProj;
    float4 camPos;
};
struct VS_Out
{
    matrix modelViewProj : MATRIX;
    float2 tex : TexCoord;
    float3 worldPos : Position;
    float3 normal : Normal;
    float4 pos : SV_Position;
};

VS_Out main(float3 pos : Position, float3 norm : Normal, float3 tan : Tangent, float biTan : BiTangent)
{
    VS_Out vso;
    vso.modelViewProj = modelViewProj;
    vso.worldPos = pos;
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.normal = norm;
    vso.tex = float2(0.0f.rr);
    return vso;
}