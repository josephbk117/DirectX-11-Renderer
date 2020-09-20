cbuffer CBuf
{
    float4 debugDrawColour;
};

float4 main() : SV_TARGET
{
    return debugDrawColour;

}