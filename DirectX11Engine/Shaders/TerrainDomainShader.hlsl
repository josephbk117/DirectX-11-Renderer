Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);

struct DS_OUTPUT
{
    float2 tex : TexCoord;
    float3 worldPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 biTan : BiTangent;
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
    float3 tan : Tangent;
    float3 biTan : BiTangent;
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
    Output.tex = patch[0].tex * domain.x + patch[1].tex * domain.y + patch[2].tex * domain.z;
    Output.worldPos = (patch[0].worldPos * domain.x + patch[1].worldPos * domain.y + patch[2].worldPos * domain.z);

    Output.normal = normalize((patch[0].normal * domain.x + patch[1].normal * domain.y + patch[2].normal * domain.z));

    uint3 texIndex = { Output.tex * 2048, 0 };
    
    float yPos = 0.0f;
    
    const int kernelSize = 5;
    
    for (int i = -kernelSize; i < kernelSize; i++)
    {
        for (int j = -kernelSize; j < kernelSize; j++)
        {
            uint3 tempIndex = { texIndex.x + i, texIndex.y + j, 0 };
            yPos += tex0.Load(tempIndex).r;
        }
    }

    yPos = yPos / ((kernelSize * 2.0f) * (kernelSize * 2.0f));
    yPos = (yPos * 2.0f) - 1.0f;
    //Output.worldPos.y = yPos * 150.0f;

    Output.vPosition = mul(float4(Output.worldPos, 1), patch[0].modelViewProj);
    tex1.Load(texIndex).xzy * 2.0f - 1.0f;
    Output.tan = (patch[0].tan * domain.x + patch[1].tan * domain.y + patch[2].tan * domain.z);
    Output.biTan = (patch[0].biTan * domain.x + patch[1].biTan * domain.y + patch[2].biTan * domain.z);
    

	return Output;
}
