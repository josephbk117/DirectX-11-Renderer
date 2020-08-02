RWTexture2D<float4> terrainTex : register(u0);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float4 sum = float4(1, 1, 1, 100);
    for (int i = -3; i < 3; ++i)
    {
        for (int j = -3; j < 3; ++j)
        {
            uint2 offset = uint2(DTid.x + i, DTid.y + j);
            sum += terrainTex[offset];
        }
    }

    terrainTex[DTid.xy] += sum * 0.0001f;

}