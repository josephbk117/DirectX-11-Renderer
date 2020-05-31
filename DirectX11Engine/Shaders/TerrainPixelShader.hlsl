Texture2D tex : register(t0);
SamplerState splr;

struct PS_In
{
    float2 tex : TexCoord;
    float3 worldPos : Position;
    float3 normal : Normal;
    float4 pos : SV_Position;
};


float4 main(PS_In psIn, uint tid : SV_PrimitiveID) : SV_Target
{
    float vdot = max(dot(normalize(float3(0, 1, 1)), normalize(psIn.normal)), 0.0f);
    float4 outCol = tex.Sample(splr, psIn.worldPos.xz * 0.03f) * vdot + tex.Sample(splr, psIn.worldPos.xz * 0.01f) * 0.1f;
    return outCol;

}