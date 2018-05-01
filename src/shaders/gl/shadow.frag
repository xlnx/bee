#version 300 es

precision mediump float;

uniform vec3 gLightWorldPos;


in vec3 WorldPos0;

out vec4 FragColor;

vec4 normfloat2vec4(float x)
{
	const vec4 bitShift = vec4(1.0, 256.0, 256.0 * 256.0, 256.0 * 256.0 * 256.0);
	const vec4 bitMask = vec4(1.0/256.0, 1.0/256.0, 1.0/256.0, 0.0);
	vec4 rgba = fract(x * bitShift);
	rgba -= rgba.gbaa * bitMask;
	return rgba;
}

void main()
{
	vec3 LightToVertex = WorldPos0 - gLightWorldPos;
	float LightToPixelDistance = length(LightToVertex);
	FragColor = normfloat2vec4(LightToPixelDistance / 1e7);
}