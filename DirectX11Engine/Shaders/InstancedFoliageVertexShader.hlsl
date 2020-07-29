Texture2D terrainTex : register(t0);

cbuffer CBuf
{
    matrix model;
    matrix view;
    matrix projection;
    matrix modelView;
    matrix modelViewProj;
    float4 camPos;
};

struct VS_Out
{
    float2 tex : TexCoord;
    
    float3 viewNorm : Normal;
    float3 viewTan : Tangent;
    float3 viewBiTan : BiTangent;
    float3 viewPos : Position;
    
    float3 tPos : Position1;
    float3 tViewPos : Position2;
    float3 lightPos : Position3;
    
    float4 pos : SV_Position;
};

uint2 dim(Texture2D textureObj)
{
    uint width;
    uint height;
    textureObj.GetDimensions(width, height);
    return uint2(width, height);
}



float4 setAxisAngle(float3 axis, float rad)
{
    rad = rad * 0.5f;
    float s = sin(rad);
    return float4(s * axis[0], s * axis[1], s * axis[2], cos(rad));
}


inline float4 rotationTo(float3 a, float3 b)
{
    const float PI = 22.0f/7.0f;
    
    float vecDot = dot(a, b);
    float3 tmpvec3 = float3(0, 0, 0);
    if (vecDot < -0.999999f)
    {
        tmpvec3 = cross(float3(1.0f, 0.0f, 0.0f), a);
        if (length(tmpvec3) < 0.000001f)
        {
            tmpvec3 = cross(float3(0.0f, 1.0f, 0.0f), a);
        }
        tmpvec3 = normalize(tmpvec3);
        return setAxisAngle(tmpvec3, PI);
    }
    else if (vecDot > 0.999999f)
    {
        return float4(0, 0, 0, 1.0f);
    }
    else
    {
        tmpvec3 = cross(a, b);
        float4 _out = float4(tmpvec3[0], tmpvec3[1], tmpvec3[2], 1.0f + vecDot);
        return normalize(_out);
    }
}


float4 multQuat(float4 q1, float4 q2)
{
    return float4(
                q1.w * q2.x + q1.x * q2.w + q1.z * q2.y - q1.y * q2.z,
                q1.w * q2.y + q1.y * q2.w + q1.x * q2.z - q1.z * q2.x,
                q1.w * q2.z + q1.z * q2.w + q1.y * q2.x - q1.x * q2.y,
                q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
              );
}


float3 rotateVector(float4 quat, float3 vec)
{
    float4 qv = multQuat(quat, float4(vec, 0.0));
    return multQuat(qv, float4(-quat.x, -quat.y, -quat.z, quat.w)).xyz;
}



VS_Out main(float3 pos : Position, float3 norm : Normal, float3 tan : Tangent, float3 biTan : BiTangent, float2 tex : TexCoord, uint instanceID : SV_InstanceID)
{
    VS_Out vso;

    const uint3 tempIndex = { ((instanceID) % 512), ((instanceID) / 512), 0 };
    float mulp = 1000.0f / 512.0f;
    
    float4 terrainData = terrainTex.Load(tempIndex);
    float3 terrainNormal = normalize(terrainData.rgb);
    float3 disp = float3((instanceID % 512) * mulp, terrainData.a, (instanceID / 512.0f) * mulp);
    
    pos = rotateVector(rotationTo(terrainNormal, float3(0, 1, 0)), pos).rgb;
    pos += terrainNormal * 1.0f;
    
    vso.pos = mul(float4(pos + disp, 1.0f), modelViewProj);
    vso.tex = tex;

    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNorm = mul(norm, (float3x3) modelView);
    vso.viewTan = mul(tan, (float3x3) modelView);
    vso.viewBiTan = mul(biTan, (float3x3) modelView);

    float3 T = normalize(mul(tan, (float3x3) model));
    float3 N = normalize(mul(norm, (float3x3) model));
    float3 B = normalize(mul(biTan, (float3x3) model));

    float3x3 TBN = transpose(float3x3(T, B, N));

    float3 fragPos = mul(float4(pos, 1.0f), model);

    vso.tPos = mul(fragPos, TBN);
    vso.tViewPos = mul(camPos.xyz, TBN);
    
    vso.lightPos = mul(float3(0.0f, 10.0f, 0.0f), TBN);

    return vso;
}