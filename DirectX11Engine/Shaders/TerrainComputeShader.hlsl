RWTexture2D<float4> terrainTex : register(u0);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float4 sum = float4(0, 0, 0, 0);
    const int lol = 1;
    for (int i = -lol; i < lol; ++i)
    {
        for (int j = -lol; j < lol; ++j)
        {
            uint2 offset = uint2(DTid.x + i, DTid.y + j);
            sum += terrainTex[offset];
        }
    }

    terrainTex[DTid.xy] = sum * (1.0f / ((lol + 1) * (lol + 1)));

}