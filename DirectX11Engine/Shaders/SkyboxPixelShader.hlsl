const float earthRadius = 6360e3;
const float atmosphereRadius = 6420e3;

bool solveQuadratic(float a, float b, float c, inout float x1, inout float x2)
{
    if (b == 0)
    {
        // Handle special case where the the two vector ray.dir and V are perpendicular
        // with V = ray.orig - sphere.centre
        if (a == 0)
            return false;
        x1 = 0;
        x2 = sqrt(-c / a);
        return true;
    }
    
    float discr = b * b - (4.0f * a * c);
 
    if (discr < 0)
        return false;
 
    float q = (b < 0.f) ? -0.5f * (b - sqrt(discr)) : -0.5f * (b + sqrt(discr));
    x1 = q / a;
    x2 = c / q;
 
    return true;
}

bool raySphereIntersect(float3 orig, float3 dir, float radius, inout float t0, inout float t1)
{
    float3 rc = float3(0, 1, 0) * radius - orig;
    float radius2 = radius * radius;
    float tca = dot(rc, dir);
    float d2 = dot(rc, rc) - tca * tca;
    if (d2 > radius2)
        return false;
    float thc = sqrt(radius2 - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    return true;
}

float3 computeIncidentLight(float3 orig, float3 dir, float tmin, float tmax)
{
    
    const float3 sunDirection = normalize(float3(0, 1, 0));
    const float M_PI = 22.0f / 7.0f;
   
    const float3 betaR = float3(3.8e-6f, 13.5e-6f, 33.1e-6f);
    const float betaM = 21e-6f;
    float Hr = 7994;
    float Hm = 1200;
    
    double t0 = 10.0f, t1 = 6000.0f;
    
    if (!raySphereIntersect(orig, dir, atmosphereRadius, t0, t1) || t1 < 0)
        return float4(0, 1, 0, 1);
    if (t0 > tmin && t0 > 0)
        tmin = t0;
    if (t1 < tmax)
        tmax = t1;
    int numSamples = 16;
    int numSamplesLight = 8;
    float segmentLength = (tmax - tmin) / numSamples;
    float tCurrent = tmin;
    float3 sumR = float3(0, 0, 0); // mie and rayleigh contribution 
    float3 sumM = float3(0, 0, 0);

    float opticalDepthR = 0, opticalDepthM = 0;
    float mu = dot(dir, sunDirection); // mu in the paper which is the cosine of the angle between the sun direction and the ray direction 
    float phaseR = 3.f / (16.f * M_PI) * (1 + mu * mu);
    float g = 0.76f;
    float phaseM = 3.f / (8.f * M_PI) * ((1.f - g * g) * (1.f + mu * mu)) / ((2.f + g * g) * pow(1.f + g * g - 2.f * g * mu, 1.5f));
    
    for (int i = 0; i < numSamples; ++i)
    {
        float3 samplePosition = orig + (tCurrent + segmentLength * 0.5f) * dir;
        float height = length(samplePosition) - earthRadius;
        // compute optical depth for light
        float hr = exp(-height / Hr) * segmentLength;
        float hm = exp(-height / Hm) * segmentLength;
        opticalDepthR += hr;
        opticalDepthM += hm;
        // light optical depth
        float t0Light = 0.0f, t1Light = 0.0f;
        raySphereIntersect(samplePosition, sunDirection, atmosphereRadius, t0Light, t1Light);
        
        float segmentLengthLight = t1Light / numSamplesLight, tCurrentLight = 0;
        float opticalDepthLightR = 0, opticalDepthLightM = 0;
        int j;
        for (j = 0; j < numSamplesLight; ++j)
        {
            float3 samplePositionLight = samplePosition + (tCurrentLight + segmentLengthLight * 0.5f) * sunDirection;
            float heightLight = length(samplePositionLight) - earthRadius;
            if (heightLight < 0)
                break;
            opticalDepthLightR += exp(-heightLight / Hr) * segmentLengthLight;
            opticalDepthLightM += exp(-heightLight / Hm) * segmentLengthLight;
            tCurrentLight += segmentLengthLight;
        }
        if (j == numSamplesLight)
        {
            float3 tau = betaR * (opticalDepthR + opticalDepthLightR) + betaM * 1.1f * (opticalDepthM + opticalDepthLightM);
            float3 attenuation = float3(exp(-tau.x), exp(-tau.y), exp(-tau.z));
            sumR += attenuation * hr;
            sumM += attenuation * hm;
        }
        tCurrent += segmentLength;
    }
 
    // We use a magic number here for the intensity of the sun (20). We will make it more
    // scientific in a future revision of this lesson/code
    return (sumR * betaR * phaseR + sumM * betaM * phaseM) * 20;
}


float4 main(float3 viewPos : Position) : SV_TARGET
{
    float3 dir = normalize(viewPos) ;
    float3 col = computeIncidentLight(float3(0, earthRadius + 1, 0), dir, 0, 999999999.0f);
    return float4(col.rgb, 1.0);

}