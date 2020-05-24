cbuffer CBuf
{
    matrix view;
    matrix modelView;
    matrix modelViewProj;
};

struct VS_Out
{
    float3 viewPos : Position;
    float4 pos : SV_Position;
};

VS_Out main(float3 pos : Position, float3 norm : Normal)
{
    VS_Out vso;
    vso.viewPos = normalize(pos); //(float3) mul(float4(pos, 0.0f), modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vso;
}