#version 300 es

precision mediump float;

// camera
uniform mat4 gWorld;
// global
uniform sampler2D gDisplacement;
uniform sampler2D gNormalJ;
uniform sampler2D gBump;
uniform sampler2D gPerlin;


in vec2 TexCoord0;
in vec3 WorldPos0;
in float Distance0;

layout (location = 1) out vec4 NormalType;
layout (location = 2) out vec4 Extra;

const float blendBias = 4.;
const float blendDist = 10.;
const float disappearBias = 15.;
const float disappearDist = 35.;

float sigmod(float x)
{
	return 1. / (1. + exp(-x));
}

void main()
{
	vec4 nj = texture(gNormalJ, TexCoord0);
	vec3 nb = texture(gBump, TexCoord0 * 3.).xyz * 2. - 1.;
	vec3 perlin = texture(gPerlin, (TexCoord0 * 2. - 1.) * sqrt(blendDist / Distance0)).xyz;
	vec3 n = mix(normalize(nb), normalize(nj.xyz), .7);
	n = mix(normalize(n), normalize(perlin), sigmod((Distance0 - blendDist) / blendBias));
	n = mix(normalize(n), vec3(0, 0, 1), sigmod((Distance0 - disappearDist) / disappearBias));
	// n = mod(vec3((TexCoord0 * 2. - 1.) * sqrt(blendDist / Distance0), 0), vec3(1.));
		// normalize(nj.xyz);
	// mix(normalize(nb), normalize(nj.xyz), .7);

	NormalType = vec4((gWorld * vec4(n, 0)).xyz, 2.);
	Extra = vec4(WorldPos0, nj.w); // sea
}
