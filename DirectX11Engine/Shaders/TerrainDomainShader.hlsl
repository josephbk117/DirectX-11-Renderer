Texture2D terrainTex : register(t0);

SamplerState terrainSplr;

struct DS_OUTPUT
{
    float2 tex : TexCoord;
    float3 worldPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 biTan : BiTangent;
    float4 vPosition : SV_POSITION;
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

cbuffer DetailInfo
{
    float maxTessellationAmount;
    float maxDistance;
    int smoothing;
};

uint2 dim(Texture2D textureObj)
{
    uint width;
    uint height;
    textureObj.GetDimensions(width, height);
    return uint2(width, height);
}

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
    DS_OUTPUT Output;
    Output.tex = patch[0].tex * domain.x + patch[1].tex * domain.y + patch[2].tex * domain.z;
    Output.worldPos = (patch[0].worldPos * domain.x + patch[1].worldPos * domain.y + patch[2].worldPos * domain.z);

    const float4 terrainSample = terrainTex.SampleLevel(terrainSplr, Output.tex, 0);
    
    Output.worldPos.y = terrainSample.a;
    Output.normal = terrainSample.rgb;

    Output.vPosition = mul(float4(Output.worldPos, 1), patch[0].modelViewProj);
    Output.tan = (patch[0].tan * domain.x + patch[1].tan * domain.y + patch[2].tan * domain.z);
    Output.biTan = (patch[0].biTan * domain.x + patch[1].biTan * domain.y + patch[2].biTan * domain.z);

    return Output;
}
