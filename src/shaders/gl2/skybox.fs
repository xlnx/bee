#version 300 es

precision mediump float;

uniform samplerCube gAmbient;


in vec3 WorldPos0;

out vec4 FragColor;

void main()
{
	FragColor = texture(gAmbient, WorldPos0);
}