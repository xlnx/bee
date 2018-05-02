#version 300 es

precision mediump float;

in vec2 Position0;

out vec4 FragColor;

void main()
{
	vec2 uv = Position0;
	FragColor = vec4(uv * .5 + .5, 0, 1);
}