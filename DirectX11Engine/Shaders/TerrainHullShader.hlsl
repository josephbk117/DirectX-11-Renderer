// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
    matrix modelViewProj : MATRIX;
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
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

cbuffer DetailInfo
{
    float tessellationAmount;
    int smoothing;
};

float TessellationEdgeFactor(VS_CONTROL_POINT_OUTPUT cp0, VS_CONTROL_POINT_OUTPUT cp1)
{
    float3 p0 = cp0.worldPos;
    float3 p1 = cp1.worldPos;
    float edgeLength = distance(p0, p1);

    float3 edgeCenter = (p0 + p1) * 0.5;
    float viewDistance = distance(edgeCenter, float3(500,10,350));

    return edgeLength / (0.05f * viewDistance);
    
    
    return tessellationAmount;
}

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;
    
	// Insert code to compute Output here
    Output.EdgeTessFactor[0] = TessellationEdgeFactor(ip[1], ip[2]);
    Output.EdgeTessFactor[2] = TessellationEdgeFactor(ip[2], ip[0]);
    Output.EdgeTessFactor[1] = TessellationEdgeFactor(ip[0], ip[1]);
    Output.InsideTessFactor = (Output.EdgeTessFactor[0] + Output.EdgeTessFactor[1] + Output.EdgeTessFactor[2]) * (1 / 3.0);

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

	// Insert code to compute Output here
    Output.modelViewProj = ip[i].modelViewProj;
    Output.tex = ip[i].tex;
    Output.worldPos = ip[i].worldPos;
    Output.normal = ip[i].normal;
    Output.tan = ip[i].tan;
    Output.biTan = ip[i].biTan;

    return Output;
}
