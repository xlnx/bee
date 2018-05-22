#version 300 es

#define PI 3.141592654

precision mediump float;

// camera
uniform mat4 gWorld;
// global
uniform sampler2D gDisplacement;
uniform sampler2D gBump;
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
	vec2 uv = Position0;
	vec2 tex = uv * .5 + .5;
	vec3 bump = (gWorld * vec4(texture(gBump, tex * 3.).xyz * 2. - 1., 0.)).xyz;
	// vec3 n = Normal0;
	NormalType = vec4(mix(normalize(bump), normalize(Normal0), .75), 2.);

	vec2 tdx = vec2(1., 0.) / gN;
	vec2 tdy = vec2(0., 1.) / gN;
	vec2 d = texture(gDisplacement, tex).xy * amplitude;
	vec2 dx = texture(gDisplacement, tex + tdx).xy * amplitude - d;
	vec2 dy = texture(gDisplacement, tex + tdy).xy * amplitude - d;

	vec2 dDdx = dx + tdx;
	vec2 dDdy = dy + tdy;

	// float t = gTime;
	// vec2 seed = vec2(t, t * .5);
	// vec2 noise = .3 * gaussian(uv, seed);
	float jacobian = (1. + dDdx.x) * (1. + dDdy.y) - dDdx.y * dDdy.x;
	float turb = max(0., 1. - jacobian - .13);
		// length(noise));

	Extra = vec4(WorldPos0, smoothstep(0., 1., turb)); // sea
}
