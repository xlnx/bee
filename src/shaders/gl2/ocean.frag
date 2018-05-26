#version 300 es

precision mediump float;

// camera
uniform mat4 gWorld;
// global
uniform sampler2D gDisplacement;
uniform sampler2D gNormalJ;
uniform sampler2D gBump;

// materials
uniform samplerCube gAmbient;
uniform float gSpecularIntensity;
uniform float gSpecularPower;


in vec2 TexCoord0;
in vec3 WorldPos0;

layout (location = 1) out vec4 NormalType;
layout (location = 2) out vec4 Extra;

void main()
{
	vec4 nj = texture(gNormalJ, TexCoord0);
	vec3 nb = texture(gBump, TexCoord0 * 3.).xyz * 2. - 1.;
	vec3 n = mix(normalize(nb), normalize(nj.xyz), .7);

	NormalType = vec4((gWorld * vec4(n, 0)).xyz, 2.);
	Extra = vec4(WorldPos0, nj.w); // sea
}
