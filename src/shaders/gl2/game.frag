#version 300 es

precision mediump float;

uniform sampler2D gImage;
uniform sampler2D gNormalType;
uniform sampler2D gExtra;
uniform samplerCube gAmbient;

uniform vec3 gCameraWorldPos;
uniform mat4 gV;
uniform mat4 gP;

#define RAYMARCH_MAX_ITER 16
#define RAYMARCH_ITER_STEP 4e-2
#define RAYMARCH_EPS .8e-2


in vec2 Position0;
in vec3 Incidence0;

out vec4 FragColor;

const float WhitecapBlend = .1;
const float InWaterBlend = .25;

// sampled from sh3
const vec4 waterColor = vec4(.24, .36, .43, 1.);

const float FresnelStep = .08;

const float FresnelBiasAbove = .02;
const float FresnelPowerAbove = -7.;
const float FresnelScaleAbove = .98;

const float FresnelBiasBelow = .03;
const float FresnelPowerBelow = -48.;
const float FresnelScaleBelow = 2e10;

const vec4 waterSurface = vec4(.1, .15, .2, 1.);

bool RaymarchVessel(vec2 uv, vec4 pw, vec3 dir, out vec3 rgb)
{
	vec2 tex = uv * .5 + .5;
	vec3 p = vec3(tex, pw.z);
	vec4 p2 = gP * (pw + vec4(dir, 1.)); p2 /= p2.w;
	vec2 cdir = p2.xy - uv;
	vec2 sdir = normalize(cdir);
	vec3 v = vec3(sdir * .5, dir.z * sdir.x / cdir.x) * RAYMARCH_ITER_STEP;

	for (int i = 0; i != RAYMARCH_MAX_ITER; ++i)
	{
		p += v;
		if (p.z < texture(gImage, p.xy).a)
		{
			p -= v; v *= .5;
		}
	}

	vec4 color = texture(gImage, p.xy);
	rgb = color.rgb;
	vec2 p1 = abs(p.xy * 2. - 1.);
	return abs(p.z - color.a) < RAYMARCH_EPS && max(p1.x, p1.y) < 1.;
}

void main()
{
	vec2 uv = Position0;
	vec2 tex = uv * .5 + .5;
	vec4 ch = texture(gImage, tex);
	float h = ch.w;
	vec4 color = vec4(ch.xyz, 1.);
	vec4 nt = texture(gNormalType, tex);
	vec3 n = normalize(nt.xyz);
	float type = nt.w;

	if (type == 2.)  // sea
	{   // sea
		bool hit;
		vec3 ve = normalize(Incidence0);
		vec3 r, t;
		float R;
		float whitecap = 0.;
		vec4 pw = gV * vec4(texture(gExtra, tex).xyz, 1.);
		

		if (gCameraWorldPos.z >= 0.)
		{
			r = reflect(-ve, n); r.z = abs(r.z);
			t = refract(-ve, n, 1.0 / 1.33);

			R = clamp(FresnelBiasAbove + FresnelScaleAbove * 
				pow(1. + dot(r, n), FresnelPowerAbove), 0., 1.);
			whitecap = texture(gExtra, uv * .5 + .5).w *
				exp(- length(pw) * WhitecapBlend);
		}
		else
		{
			n = -n;

			r = reflect(-ve, n); r.z = -abs(r.z);
			t = refract(-ve, n, 1.33 / 1.);
			
			R = clamp(FresnelBiasBelow + FresnelScaleBelow * 
				pow(1. + dot(r, n), FresnelPowerBelow), 0., 1.);
		}

		vec3 vessel;
		vec4 rcolor, tcolor;

		if (R > FresnelStep && gCameraWorldPos.z >= 0. &&
			RaymarchVessel(uv, pw, (gV * vec4(r, 0.)).xyz, vessel))
		{
			rcolor = mix(vec4(-.1), vec4(vessel, 1.), .6);
		}
		else
		{
			rcolor = texture(gAmbient, r) * 1.2;
		}

		// if (R < 1. - FresnelStep && RaymarchVessel(uv, pw, (gV * vec4(t, 0.)).xyz, vessel))
		// {
		// 	tcolor = mix(waterSurface, waterSurface + vec4(vessel, 1.), 0.8);
		// }
		// else
		{
			tcolor = texture(gAmbient, t);
		}

		color = R * rcolor + (1.0 - R) * tcolor + whitecap;
	}
	else if (type == 1.)   // vessel
	{
		vec3 ve = Incidence0;
		vec3 r = reflect(-ve, n);
		
		const float c1 = 0.1;
		vec4 amb = c1 * texture(gAmbient, r);

		if (gCameraWorldPos.z > 0.)
		{
			color = amb + mix(color, amb, .2);
		}
		else
		{
			color = mix(color, waterColor, clamp(- h * InWaterBlend, .01, 1.));
		}
	}
	FragColor = color;
}
