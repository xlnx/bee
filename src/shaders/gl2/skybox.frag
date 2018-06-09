#version 300 es

#define EPS (2.)

precision mediump float;

uniform samplerCube gAmbient;
uniform vec3 gCameraWorldPos;


in vec3 WorldPos0;

layout (location = 0) out vec3 FragColor;
layout (location = 2) out vec4 Extra;

void main()
{
	vec3 p = WorldPos0;
	Extra.w = p.z < 0. ? 0. : 1.;
	if (abs(p.z) < EPS) p.z = sign(gCameraWorldPos.z) * EPS;
	FragColor = texture(gAmbient, p).xyz;
}