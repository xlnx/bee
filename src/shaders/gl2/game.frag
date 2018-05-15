#version 300 es

precision mediump float;

uniform sampler2D gImage;
uniform sampler2D gNormalDepth;
uniform mat4 gP;

uniform vec3 gCameraWorldPos;

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
	float d;
};

vec3 getPointNormal(vec2 uv)
{
	return texture(gNormalDepth, (uv + 1.) * .5).rgb;
}

float linearlizeDepth(vec2 uv)
{
	return - gP[3].z / (1. - texture(gNormalDepth, (uv + 1.) * .5).a);
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
	info.d = length(refinePoint(ray.uv) - refinePoint(p.xy));
	info.hit = d < RAYMARCH_EPS && info.d > RAYMARCH_MIND;
	return info;
}

vec3 SSR(vec2 uv, out bool hit)
{
	vec3 N = getPointNormal(uv);
	vec3 I = Incidence0;
	vec3 R = reflect(I, N);
	hit = false;

	if (texture(gNormalDepth, (uv + 1.) * .5).a != 0.)
	{
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
				if (pow(clamp(dot(getPointNormal(hinfo.uv), N), 0., 1.), 5.) < .05)
				{
					hit = true;
					float invd = 1. / hinfo.d;
					color += texture(gImage, hinfo.uv *.5 + .5).xyz * w[i] * 
						clamp(invd * invd, 0., 1.);
					// color += vec3(1.) * invd * invd;
					// color += vec3(1.) * abs(hinfo.d);
					// color += vec3(hinfo.uv - uv, 0.) * .5 + .5;
				}
			}
		}
		
		return color;// / 5.;
	}
}

void main()
{
	vec3 bg = texture(gImage, Position0 * .5 + .5).xyz;
	bool hit;

	vec3 ssr = SSR(Position0, hit);
	if (hit)
	{
		FragColor = vec4(mix(bg, bg + ssr, 0.8), 1);
	}
	else
	{
		FragColor = vec4(bg, 1);
	}
	// FragColor = vec4(ssr, 1);
}
