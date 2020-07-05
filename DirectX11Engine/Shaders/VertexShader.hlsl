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
	float2 tex : TexCoord;
    
    float3 viewNorm : Normal;
    float3 viewTan : Tangent;
    float3 viewBiTan : BiTangent;
    float3 viewPos : Position;
    
    float3 tPos : Position1;
    float3 tViewPos : Position2;
    float3 lightPos : Position3;
    
	float4 pos : SV_Position;
};

VS_Out main(float3 pos : Position, float3 norm : Normal, float3 tan : Tangent, float3 biTan : BiTangent, float2 tex : TexCoord)
{
    VS_Out vso;
    
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tex = tex;
        
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNorm = mul(norm, (float3x3) modelView);
    vso.viewTan = mul(tan, (float3x3) modelView);
    vso.viewBiTan = mul(biTan, (float3x3) modelView);
        
    float3 T = normalize(mul(tan, (float3x3) model));
    float3 N = normalize(mul(norm, (float3x3) model));
    float3 B = normalize(mul(biTan, (float3x3) model));
    
    float3x3 TBN = transpose(float3x3(T, B, N));
    
    float3 fragPos = mul(float4(pos, 1.0f), model);
    
    vso.tPos = mul(fragPos, TBN);
    vso.tViewPos = mul(camPos.xyz, TBN);
    vso.lightPos = mul(float3(0.0f, 10.0f, 0.0f), TBN);
    
	return vso;
}