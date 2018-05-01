#version 300 es

precision mediump float;

uniform mat4 gV;


in vec3 Normal0;

out vec3 FragColor;

void main()
{
	FragColor = normalize((gV * vec4(Normal0, 0)).xyz) * .5 + .5;
}