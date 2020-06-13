struct DS_OUTPUT
{
    float2 tex : TexCoord;
    float3 worldPos : Position;
    float3 normal : Normal;
	float4 vPosition  : SV_POSITION;
	// TODO: change/add other stuff
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
    matrix modelViewProj : MATRIX;
    float2 tex : TexCoord;
    float3 worldPos : Position;
    float3 normal : Normal;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

    Output.vPosition = mul(float4(patch[0].worldPos * domain.x + patch[1].worldPos * domain.y + patch[2].worldPos * domain.z, 1), patch[0].modelViewProj);

    Output.normal = (patch[0].normal * domain.x + patch[1].normal * domain.y + patch[2].normal * domain.z);
    Output.tex = patch[0].tex;
    Output.worldPos = (patch[0].worldPos * domain.x + patch[1].worldPos * domain.y + patch[2].worldPos * domain.z);

	return Output;
}
