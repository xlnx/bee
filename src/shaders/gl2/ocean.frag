#version 300 es

#define PI 3.141592654

precision mediump float;

// global
uniform sampler2D gDisplacement;
uniform float gTime;

// materials
uniform samplerCube gAmbient;
uniform float gSpecularIntensity;
uniform float gSpecularPower;


in float Jacobian0;
in vec2 Position0;
in vec3 Normal0;
in vec3 WorldPos0;

layout (location = 1) out vec4 NormalType;
layout (location = 2) out vec4 Extra;

const float gN = 256.;
const float amplitude = 2e-3;

float UVRandom(vec2 uv, float salt, float random)
{
	uv += vec2(salt, random);
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 gaussian(vec2 uv, vec2 seed)
{
	float rand1 = UVRandom(uv, 10.612, seed.x);
	float rand2 = UVRandom(uv, 11.899, seed.y);
	float x = sqrt(2. * log(rand1 + 1.));
	float y = 2. * PI * rand2;
	return x * vec2(cos(y), sin(y)) * .5 + .2; 
}

void main()
{
	vec3 n = normalize(Normal0);
	NormalType = vec4(n, 2.);

	vec2 uv = Position0;
	vec2 tex = uv * .5 + .5;
	vec2 tdx = vec2(1., 0.) / gN;
	vec2 tdy = vec2(0., 1.) / gN;
	vec2 d = texture(gDisplacement, tex).xy * amplitude;
	vec2 dx = texture(gDisplacement, tex + tdx).xy * amplitude;
	vec2 dy = texture(gDisplacement, tex + tdy).xy * amplitude;

	vec2 dDdx = dx + tdx - d;
	vec2 dDdy = dy + tdy - d;

	float t = gTime;
	float jacobian = (1. + dDdx.x) * (1. + dDdy.y) - dDdx.y * dDdy.x;
	vec2 seed = vec2(t, t * .5);
	vec2 noise = .3 * gaussian(uv, seed);//vec2(.2); //vec2(1., 1.);
	float turb = max(0., 1. - jacobian - .13);
		// length(noise));

	Extra = vec4(WorldPos0, smoothstep(0., 1., turb)); // sea
}
