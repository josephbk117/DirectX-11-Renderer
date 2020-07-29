RWTexture2D<float> terrainTex;

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    terrainTex[DTid.xy] = float4(1, 1, 1, 0);

}