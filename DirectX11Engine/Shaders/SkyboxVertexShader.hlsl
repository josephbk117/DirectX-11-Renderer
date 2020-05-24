cbuffer CBuf
{
    matrix view;
    matrix projection;
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
   
    float4x4 translationRemovedView = view;
    translationRemovedView._41_42_43 = float3(0, 0, 0); // Zero out translation components
    vso.pos = mul(float4(pos, 1.0f), mul(translationRemovedView, projection));
    return vso;
}