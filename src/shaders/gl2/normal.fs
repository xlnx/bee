#version 300 es

precision mediump float;

in vec3 Normal0;

out vec3 FragColor;

void main()
{
	FragColor = normalize(Normal0);
}