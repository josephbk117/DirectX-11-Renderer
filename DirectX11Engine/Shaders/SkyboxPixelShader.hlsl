#define PI 3.14159265359

cbuffer CSkyInfo
{
    // thickness of the atmosphere if its density were uniform
    float hR = 7994.0; // Rayleigh
    float hM = 1200.0; // Mie
    float height = 1.0f; // Height from surface
    float sun_power = 20.0f; // Sun intensity
    float3 sun_dir = float3(0,1,0);
};

struct ray_t
{
    float3 origin;
    float3 direction;
};
#define BIAS 1e-4 // small offset to avoid self-intersections

struct sphere_t
{
    float3 origin;
    float radius;
    int material;
};

struct plane_t
{
    float3 direction;
    float distance;
    int material;
};

ray_t get_primary_ray( const in float3 cam_local_point, inout float3 cam_origin, inout float3 cam_look_at )
{
    float3 fwd = normalize(cam_look_at - cam_origin);
    float3 up = float3(0, 1, 0);
    float3 right = cross(up, fwd);
    up = cross(fwd, right);

    ray_t r;
    r.origin = cam_origin;
    r.direction = normalize(fwd + up * cam_local_point.y + right * cam_local_point.x);

    return r;
}

bool isect_sphere(const in ray_t ray, const in sphere_t sphere, inout float t0, inout float t1)
{
    float3 rc = sphere.origin - ray.origin;
    float radius2 = sphere.radius * sphere.radius;
    float tca = dot(rc, ray.direction);
    float d2 = dot(rc, rc) - tca * tca;
    if (d2 > radius2)
        return false;
    float thc = sqrt(radius2 - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    return true;
}

// scattering coefficients at sea level (m)
static const float3 betaR = float3(5.5e-6, 13.0e-6, 22.4e-6); // Rayleigh 
static const float3 betaM = float3(21e-6, 21e-6, 21e-6); // Mie

// scale height (m)
// thickness of the atmosphere if its density were uniform
//static const float hR = 7994.0; // Rayleigh
//static const float hM = 1200.0; // Mie

float rayleigh_phase_func(float mu)
{
    return
			3. * (1. + mu * mu)
	/ //------------------------
				(16. * PI);
}

// Henyey-Greenstein phase function factor [-1, 1]
// represents the average cosine of the scattered directions
// 0 is isotropic scattering
// > 1 is forward scattering, < 1 is backwards
static const float g = 0.76;
float henyey_greenstein_phase_func(float mu)
{
    return
						(1. - g * g)
	/ //---------------------------------------------
		((4. * PI) * pow(1. + g * g - 2. * g * mu, 1.5));
}

// Schlick Phase Function factor
// Pharr and  Humphreys [2004] equivalence to g above
static const float k = 1.55 * g - 0.55 * (g * g * g);

static const float earth_radius = 6360e3; // (m)
static const float atmosphere_radius = 6420e3; // (m)

//static const float sun_power = 20.0;

static const sphere_t atmosphere = { float3(0.0f, 0.0f, 0.0f), atmosphere_radius, 0 };

static const int num_samples = 16;
static const int num_samples_light = 8;

bool get_sun_light(const in ray_t ray, inout float optical_depthR, inout float optical_depthM)
{
    float t0, t1;
    isect_sphere(ray, atmosphere, t0, t1);

    float march_pos = 0.;
    float march_step = t1 / float(num_samples_light);

    for (int i = 0; i < num_samples_light; i++)
    {
        float3 s = ray.origin + ray.direction * (march_pos + 0.5 * march_step);
        float height = length(s) - earth_radius;
        if (height < 0.)
            return false;

        optical_depthR += exp(-height / hR) * march_step;
        optical_depthM += exp(-height / hM) * march_step;

        march_pos += march_step;
    }

    return true;
}

float3 get_incident_light(const in ray_t ray, float3 sun_dir)
{
	// "pierce" the atmosphere with the viewing ray
    float t0, t1;
    if (!isect_sphere(ray, atmosphere, t0, t1))
    {
        return float3(0.0f, 0.0f, 0.0f);
    }

    float march_step = t1 / float(num_samples);

	// cosine of angle between view and light directions
    float mu = dot(ray.direction, sun_dir);

	// Rayleigh and Mie phase functions
	// A black box indicating how light is interacting with the material
	// Similar to BRDF except
	// * it usually considers a single angle
	//   (the phase angle between 2 directions)
	// * integrates to 1 over the entire sphere of directions
    float phaseR = rayleigh_phase_func(mu);
    float phaseM = henyey_greenstein_phase_func(mu);


	// optical depth (or "average density")
	// represents the accumulated extinction coefficients
	// along the path, multiplied by the length of that path
    float optical_depthR = 0.;
    float optical_depthM = 0.;

    float3 sumR = float3(0, 0, 0);
    float3 sumM = float3(0, 0, 0);
    float march_pos = 0.;

    [unroll]
    for (int i = 0; i < num_samples; i++)
    {
        float3 s = ray.origin + ray.direction * (march_pos + 0.5 * march_step);
        float height = length(s) - earth_radius;

		// integrate the height scale
        float hr = exp(-height / hR) * march_step;
        float hm = exp(-height / hM) * march_step;
        optical_depthR += hr;
        optical_depthM += hm;

		// gather the sunlight
        ray_t light_ray;
        light_ray.origin = s;
        light_ray.direction = sun_dir;
        
        float optical_depth_lightR = 0.;
        float optical_depth_lightM = 0.;
        
        bool overground = get_sun_light( light_ray, optical_depth_lightR, optical_depth_lightM);

        if (overground)
        {
            float3 tau = betaR * (optical_depthR + optical_depth_lightR) + betaM * 1.1 * (optical_depthM + optical_depth_lightM);
            float3 attenuation = exp(-tau);

            sumR += hr * attenuation;
            sumM += hm * attenuation;
        }

        march_pos += march_step;
    }

    return sun_power * (sumR * phaseR * betaR + sumM * phaseM * betaM);
}

float4 main(float3 viewPos : Position) : SV_Target
{    
    viewPos = normalize(viewPos);
    float3 col = float3(0,0,0);

    float3 eye = float3(0, earth_radius + height, 0);

    ray_t ray;
    ray.origin = eye;
    ray.direction = viewPos;

    if (dot(ray.direction, float3(0, 1, 0)) > .0)
    {
        col = get_incident_light(ray, normalize(sun_dir));
    }
    else
    {
        col = float3(0.333f, 0.333f, 0.333f);
    }

    return float4(col, 1);
}