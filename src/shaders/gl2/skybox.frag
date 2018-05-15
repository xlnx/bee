#version 300 es

precision mediump float;

uniform samplerCube gAmbient;


in vec3 WorldPos0;

layout (location = 0) out vec4 FragColor;
layout (location = 2) out float Type;

void main()
{
	FragColor = texture(gAmbient, WorldPos0);
	Type = 2.; // ambient
}