#version 300 es

precision mediump float;

uniform sampler2D gImage;
uniform sampler2D gNormalDepth;
uniform sampler2D gPositionType;
uniform samplerCube gAmbient;

uniform vec3 gCameraWorldPos;
uniform mat4 gV;
uniform mat4 gP;

#define RAYMARCH_MAX_ITER 16
#define RAYMARCH_ITER_STEP 3e-2
#define RAYMARCH_EPS 1e-2
#define RAYMARCH_MIND 1e-3


in vec2 Position0;
in vec3 Incidence0;

out vec4 FragColor;

struct Ray
{
	vec2 uv;
	vec3 dir;
};

struct HitInfo
{
	vec2 uv;
	bool hit;
};

const float FresnelBiasAbove = .02;
const float FresnelPowerAbove = -7.;
const float FresnelScaleAbove = .98;

const float FresnelBiasBelow = .03;
const float FresnelPowerBelow = -48.;
const float FresnelScaleBelow = 2e10;

const vec4 waterSurface = vec4(.1, .15, .2, 1.);

vec3 getPointNormal(vec2 uv)
{
	return normalize(gV * vec4(texture(gNormalDepth, uv * .5 + .5).rgb, 0.)).xyz;
}

float linearlizeDepth(vec2 uv)
{
	return - gP[3].z / (1. - texture(gNormalDepth, uv *.5 + .5).a);
}

vec3 refinePoint(vec2 uv)
{
	float z = linearlizeDepth(uv);
	return vec3(uv.x / gP[0].x, uv.y / gP[1].y, -1.) * z;
}

HitInfo Raymarch(Ray ray)
{
	vec4 p2 = gP * vec4(refinePoint(ray.uv) + ray.dir, 1.);
	p2 /= p2.w;
	vec2 cdir = p2.xy - ray.uv;
	vec2 sdir = normalize(cdir);
	vec3 v = vec3(sdir, - ray.dir.z * sdir.x / cdir.x) * RAYMARCH_ITER_STEP;
	vec3 p = vec3(ray.uv, linearlizeDepth(ray.uv));

	for (int i = 0; i != RAYMARCH_MAX_ITER; ++i)
	{
		p += v;
		if (p.z > linearlizeDepth(p.xy))
		{
			p -= v; v *= .5;
		}
		if (p.x > 1. || p.x < -1. || p.y > 1. || p.y < -1.)
		{
			p.z = 1e7; break;
		}
	}

	float d = abs(p.z - linearlizeDepth(p.xy));
	HitInfo info;
	info.uv = p.xy;
	info.hit = d < RAYMARCH_EPS;
	return info;
}

vec3 SSR(vec2 uv, out bool hit)
{
	vec3 N = getPointNormal(uv);
	vec3 I = Incidence0;
	vec3 R = reflect(I, N);
	hit = false;

	const float scale = .15;
	const float dx[5] = float[5]( 0., 1., -1., 0., 0. );
	const float dy[5] = float[5]( 0., 0., 0., 1., -1. );
	const float w[5] = float[5]( .5, .2, .2, .2, .2 );
	
	Ray ray;
	ray.uv = uv;
	vec3 color = vec3(0.);

	for (int i = 0; i != 1; ++i)
	{
		ray.dir = R + vec3(dx[i], dy[i], 0.) * scale;
		HitInfo hinfo = Raymarch(ray);

		if (hinfo.hit)
		{
			if (texture(gPositionType, hinfo.uv * .5 + .5).w == 1.) // is vessel
			{
				hit = true;
				color += texture(gImage, hinfo.uv *.5 + .5).xyz;
			}
		}
	}
	
	return color;// / 5.;
}

#define RAYMARCH_SHIPWAVE_MAX_ITER 8

vec2 RayMarchShipWave(vec2 uv, out bool hit)
{
	hit = false;

	const float scale = .015;
	const float dx[4] = float[4]( 1., -1., 0., 0. );
	const float dy[4] = float[4]( 0., 0., 1., -1. );

	for (int i = 0; i != 4; ++i)
	{
		vec2 p = uv;
		for (int j = 0; j != RAYMARCH_SHIPWAVE_MAX_ITER; ++j)
		{
			p += vec2(dx[i], dy[i]) * scale;
			if (texture(gPositionType, p * .5 + .5).w == 1.)
			{
				hit = true;
				return p;
			}
		}
	}
}

void main()
{
	vec2 uv = Position0;
	vec2 tex = uv * .5 + .5;
	vec4 color = texture(gImage, tex);
	vec4 nd = texture(gNormalDepth, tex);
	vec4 pt = texture(gPositionType, tex);
	
	vec3 n = normalize(nd.xyz);
	vec3 p = pt.xyz;
	float type = pt.w;

	if (type == 0.)
	{   // sea
		bool hit;
		vec3 ve = normalize(gCameraWorldPos - p);
		vec3 r, t;
		float R;

		if (gCameraWorldPos.z >= 0.)
		{
			r = reflect(-ve, n); r.z = abs(r.z);
			t = refract(-ve, n, 1.0 / 1.33);

			R = clamp(FresnelBiasAbove + FresnelScaleAbove * 
				pow(1. + dot(r, n), FresnelPowerAbove), 0., 1.);
		}
		else
		{
			n = -n;

			r = reflect(-ve, n); r.z = -abs(r.z);
			t = refract(-ve, n, 1.33 / 1.);
			
			R = clamp(FresnelBiasBelow + FresnelScaleBelow * 
				pow(1. + dot(r, n), FresnelPowerBelow), 0., 1.);
		}
		
		vec3 ssr = SSR(uv, hit);
		if (hit)
		{
			color = R * mix(waterSurface, waterSurface + vec4(ssr, 1.), 0.8) + (1.0 - R) * texture(gAmbient, t);
		}
		else
		{
			color = R * texture(gAmbient, r) * 1.2 + (1.0 - R) * texture(gAmbient, t);
		}
	}
	else if (type == 1.)
	{   // vessel
		vec3 ve = gCameraWorldPos - p;
		vec3 r = reflect(-ve, n);
		
		const float c1 = 0.1;
		vec4 amb = c1 * texture(gAmbient, r);

		color = amb + mix(color, amb, .2);
	}
	FragColor = color;
}
