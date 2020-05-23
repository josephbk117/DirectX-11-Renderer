cbuffer CBuf
{
    matrix view;
    matrix modelView;
    matrix modelViewProj;
};

float4 main(float3 pos : Position, float3 norm : Normal, float3 tan : Tangent, float3 biTan : BiTangent, float2 tex : TexCoord) : SV_Position
{
    return mul(float4(pos + (norm * 1.5f), 1.0f), modelViewProj);
}