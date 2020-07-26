Texture2D terrainTex : register(t0);

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

    const float2 texDim = dim(terrainTex);
    const uint3 texIndex = { Output.tex * texDim, 0 };

    float yPos = 0.0f;

    const int kernelSize = min(8, smoothing);

    float3 outNorm;

    for (int i = -kernelSize; i < kernelSize; i++)
    {
        for (int j = -kernelSize; j < kernelSize; j++)
        {
            const uint3 tempIndex = { texIndex.x + i, texIndex.y + j, 0 };
            yPos += terrainTex.Load(tempIndex).a;
            outNorm.rgb += terrainTex.Load(tempIndex).rgb;
        }
    }

   float kernelSizeDivide = 1.0f / ((kernelSize * 2.0f) * (kernelSize * 2.0f));

    yPos = yPos * kernelSizeDivide;
    Output.worldPos.y = yPos;
    Output.normal = normalize(outNorm * kernelSizeDivide);

    Output.vPosition = mul(float4(Output.worldPos, 1), patch[0].modelViewProj);
    Output.tan = (patch[0].tan * domain.x + patch[1].tan * domain.y + patch[2].tan * domain.z);
    Output.biTan = (patch[0].biTan * domain.x + patch[1].biTan * domain.y + patch[2].biTan * domain.z);

    return Output;
}
