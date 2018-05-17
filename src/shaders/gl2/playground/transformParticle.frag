#version 300 es

precision mediump float;

in vec3 Position_next;
in vec3 Color_next;

out vec4 FragColor;

void main()
{
	FragColor = vec4(Color_next, 1);
}