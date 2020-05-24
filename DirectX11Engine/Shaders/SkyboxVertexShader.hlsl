cbuffer CBuf
{
    matrix view;
    matrix modelView;
    matrix modelViewProj;
};

struct VS_Out
{
    float3 viewDir : Position;
    float4 pos : SV_Position;
};

VS_Out main(float3 pos : Position)
{
    VS_Out vso;
    vso.viewDir = normalize(pos);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vso;
}