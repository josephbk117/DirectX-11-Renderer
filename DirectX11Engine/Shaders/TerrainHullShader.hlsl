// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
    matrix modelViewProj : MATRIX;
    float tessFactor : TexCoord1;
    float2 tex : TexCoord;
    float3 worldPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 biTan : BiTangent;
	// TODO: change/add other stuff
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
    matrix modelViewProj : MATRIX;
    float2 tex : TexCoord;
    float3 worldPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 biTan : BiTangent;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor; // e.g. would be [4] for a quad domain
    float InsideTessFactor : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

#define NUM_CONTROL_POINTS 3

cbuffer DetailInfo
{
    float maxTessellationAmount;
    float maxDistance;
    int smoothing;
};

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

    Output.EdgeTessFactor[0] = 0.5f * (ip[1].tessFactor + ip[2].tessFactor);
    Output.EdgeTessFactor[1] = 0.5f * (ip[2].tessFactor + ip[0].tessFactor);
    Output.EdgeTessFactor[2] = 0.5f * (ip[0].tessFactor + ip[1].tessFactor);
    Output.InsideTessFactor = Output.EdgeTessFactor[0];
    
    return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONTROL_POINT_OUTPUT Output;

    Output.modelViewProj = ip[i].modelViewProj;
    Output.tex = ip[i].tex;
    Output.worldPos = ip[i].worldPos;
    Output.normal = ip[i].normal;
    Output.tan = ip[i].tan;
    Output.biTan = ip[i].biTan;

    return Output;
}
