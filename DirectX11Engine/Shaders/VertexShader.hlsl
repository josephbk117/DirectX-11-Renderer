cbuffer CBuf
{
    matrix view;
    matrix projection;
    matrix modelView;
    matrix modelViewProj;
};

struct VS_Out
{
	float2 tex : TexCoord;
    float3 viewNorm : Normal;
    float3 viewTan : Tangent;
    float3 viewBiTan : BiTangent;
    float3 viewPos : Position;
	float4 pos : SV_Position;
};

VS_Out main(float3 pos : Position, float3 norm : Normal, float3 tan : Tangent, float3 biTan : BiTangent, float2 tex : TexCoord)
{
    VS_Out vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNorm = mul(norm, (float3x3) modelView);
    vso.viewTan = mul(tan, (float3x3) modelView);
    vso.viewBiTan = mul(biTan, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tex = tex;
	return vso;
}