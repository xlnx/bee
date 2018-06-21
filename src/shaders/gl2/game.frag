#version 300 es

precision mediump float;

uniform sampler2D gImage;
uniform sampler2D gNormalType;
uniform sampler2D gExtra;
uniform sampler2D gSmoke;
uniform samplerCube gAmbient;

uniform vec3 gLightDir;
uniform vec3 gCameraWorldPos;
uniform mat4 gV;
uniform mat4 gP;

#define RAYMARCH_MAX_ITER 24
#define RAYMARCH_ITER_STEP 6e-2
#define RAYMARCH_EPS .1e-2

#define REFLECT_Z_EPS 1e-2
#define CAMERA_Z_EPS 5e-2

#define PI_2 (3.141592654 * .5)

in vec2 Position0;
in vec3 Incidence0;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out float Stencial;

const float whitecapBlend = .1;
const float waterAtten = 2.0e-1;

const float FresnelStep = .08;

const float FresnelBiasAbove = .02;
const float FresnelPowerAbove = -7.;
const float FresnelScaleAbove = .98;

const float FresnelBiasBelow = .03;
const float FresnelPowerBelow = -48.;
const float FresnelScaleBelow = 2e10;

const vec4 waterSurface = vec4(.1, .15, .2, 1.);

const vec4 shallowWaterColor = vec4(.24, .36, .43, 1.);
const vec4 shallowWaterColorNight = vec4(.23, .28, .33, 1.);

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
	vec4 nt = texture(gNormalType, tex);
	vec4 ex = texture(gExtra, tex);

	float h = ch.w;
	vec4 color = vec4(ch.xyz, 1.);
	vec3 n = normalize(nt.xyz);
	float type = nt.w;

	float ang = dot(-gLightDir, vec3(0, 0, 1));

	vec4 mixColor = mix(shallowWaterColorNight, shallowWaterColor, ang);
	float waterBlend = clamp(sqrt((abs(h) + 1.) * waterAtten) - .5, .5, 1.);

	Stencial = ex.w;

	if (type == 2.)  // sea
	{   // sea
		bool hit;
		vec3 ve = normalize(Incidence0);
		vec3 r, t;
		float R;
		float whitecap = 0.;
		vec4 pw = gV * vec4(ex.xyz, 1.);
		
		float f = dot(ve, n);
		if (abs(f) < .03)
		{
			f = sign(gCameraWorldPos.z);
		}
		if (gCameraWorldPos.z > CAMERA_Z_EPS || 
			gCameraWorldPos.z > -CAMERA_Z_EPS && f > 0.)
		{
			r = reflect(-ve, n); r.z = abs(r.z);
			t = refract(-ve, n, 1.0 / 1.33);

			R = clamp(FresnelBiasAbove + FresnelScaleAbove * 
				pow(1. + dot(r, n), FresnelPowerAbove), 0., 1.);
			whitecap = ex.w * exp(- length(pw) * whitecapBlend);
		}
		else
		{
			n = -n;

			r = reflect(-ve, n); r.z = -abs(r.z);
			t = refract(-ve, n, 1.33 / 1.);
			
			R = clamp(FresnelBiasBelow + FresnelScaleBelow * 
				pow(1. + dot(r, n), FresnelPowerBelow), 0., 1.);
		}

		float deye = dot(-gLightDir, r);
		float spec = pow(max(deye, 0.), 120.) * 1.6;
		vec3 vessel;
		vec4 rcolor = vec4(spec); 
		vec4 tcolor = texture(gAmbient, t);

		if (R > FresnelStep && gCameraWorldPos.z >= 0. &&
			RaymarchVessel(uv, pw, (gV * vec4(r, 0.)).xyz, vessel))
		{
			rcolor += mix(vec4(-.1), vec4(vessel, 1.), .6);
		}
		else
		{
			if (abs(r.z) < REFLECT_Z_EPS) r.z = sign(r.z) * REFLECT_Z_EPS;
			rcolor += texture(gAmbient, r);// * 1.2;
		}

		color = R * rcolor + (1.0 - R) * tcolor + whitecap;
		if (gCameraWorldPos.z < 0.)
		{
			vec4 mixColor = mix(shallowWaterColorNight, shallowWaterColor, ang);
			color = mix(color, mixColor, clamp(-pw.z * waterAtten, 0., 1.));
		}
		// color = vec4(n * .5 + .5, 1.);
		// color = nt.xyzz;
		Stencial = n.z < 0. ? 0. : 1.;
	}
	else if (type == 1.)   // vessel
	{
		vec3 ve = Incidence0;
		vec3 r = reflect(-ve, n);
		
		// const float c1 = .65;
		const float difuamp = .85;
		const float specamp = .3;
		const float specpow = 5.;

		float c1 = sin(ang * PI_2) * difuamp;
		vec4 amb = texture(gAmbient, r.z <= 0. ? r * vec3(1, 1, -1) : r);

		float difu = max(dot(n, -gLightDir), 0.);

		vec3 hv = (ve + -gLightDir) * .5;
		float spec = specamp * pow(dot(hv, n), specpow);

		color = mix(.25 * color, color, (.95 - c1) * difu + c1 * (amb.x + amb.y + amb.z) / 3.) + spec;

		if (Stencial == 0. || gCameraWorldPos.z < -CAMERA_Z_EPS)
		{
			color = mix(color, mixColor, waterBlend);
		}
	}
	vec4 smoke = texture(gSmoke, tex);
	FragColor = mix(color, smoke, smoke.a);
}
