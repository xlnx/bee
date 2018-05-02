#version 300 es

precision mediump float;

uniform sampler2D gImage;
uniform sampler2D gNormal;
uniform sampler2D gDepth;
uniform mat4 gP;

uniform vec3 gCameraWorldPos;

// #define RAYMARCH_MAX_ITER 200
// #define RAYMARCH_ITER_STEP 1e-2
// #define RAYMARCH_EPS 1.5e-15

#define RAYMARCH_MAX_ITER 12
#define RAYMARCH_ITER_STEP 6e-2
#define RAYMARCH_EPS 2e-4


in vec2 Position0;
in vec3 Incidence0;

out vec4 FragColor;

struct PointInfo
{
	vec3 normal;
	float depth;
};

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

float vec42normfloat(vec4 v)
{
	const vec4 bitShift = vec4(1.0, 1.0/256.0, 1.0/(256.0*256.0), 1.0/(256.0*256.0*256.0));
	return dot(v, bitShift);
}

float decodeDepth(float d)
{
	return sqrt(- log(d) - 1.);
	return d;
}

float getPointDepth(vec2 uv)
{
	return decodeDepth(vec42normfloat(texture(gDepth, (uv + 1.) * .5)));
}

PointInfo decodePoint(vec2 uv)
{
	PointInfo info;
	info.normal = texture(gNormal, (uv + 1.) * .5).xyz * 2. - 1.;
	info.depth = getPointDepth(uv);
	// vec42normfloat(texture(gDepth, (uv + 1.) * .5));
	return info;
}

HitInfo Raymarch(Ray ray)
{
	vec4 cdir = gP * vec4(ray.dir, 0.);
	cdir *= vec4(1. / abs(cdir.w), 1. / abs(cdir.w), -sign(cdir.z), 0.);
	vec2 sdir = normalize(cdir.xy);
	vec3 v = vec3(sdir, cdir.z * sdir.x / cdir.x) * RAYMARCH_ITER_STEP;
	vec3 p = vec3(ray.uv, getPointDepth(ray.uv));

	for (int i = 0; i != RAYMARCH_MAX_ITER; ++i)
	{
		p += v;
		if (p.z > getPointDepth(p.xy))
		{
			p -= v; v *= .5;
		}
		if (p.x > 1. || p.x < -1. || p.y > 1. || p.y < -1.)
		{
			p.z = 1e7; break;
		}
	}

	float d = abs(p.z - getPointDepth(p.xy));
	HitInfo info;
	info.uv = p.xy;
	info.hit = d < RAYMARCH_EPS;
	return info;
}

void main()
{
	vec2 uv = Position0;
	PointInfo pinfo = decodePoint(uv);
	vec3 N = pinfo.normal;
	vec3 I = Incidence0;
	vec3 R = reflect(I, N);

	if (pinfo.depth != 0.)
	{
		Ray ray;
		ray.uv = uv;
		ray.dir = R;
		HitInfo hinfo = Raymarch(ray);

		if (hinfo.hit)
		{
			vec4 color = texture(gImage, hinfo.uv *.5 + .5);
			FragColor = mix(texture(gImage, uv *.5 + .5), color, .6);
		}
		else
		{
			FragColor = texture(gImage, uv *.5 + .5);
		}
	}
	else
	{
		FragColor = texture(gImage, uv *.5 + .5);
	}
}
