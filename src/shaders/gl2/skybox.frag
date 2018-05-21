#version 300 es

precision mediump float;

uniform samplerCube gAmbient;


in vec3 WorldPos0;

layout (location = 0) out vec4 FragColor;
layout (location = 2) out vec4 PositionType;

void main()
{
	FragColor = texture(gAmbient, WorldPos0);
	PositionType = vec4(WorldPos0, 2.); // ambient
}