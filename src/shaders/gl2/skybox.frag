#version 300 es

precision mediump float;

uniform samplerCube gAmbient;


in vec3 WorldPos0;

layout (location = 0) out vec3 FragColor;
layout (location = 2) out vec4 PositionType;

void main()
{
	FragColor = vec3(texture(gAmbient, WorldPos0).xyz);
	PositionType = vec4(WorldPos0, 2.); // ambient
}