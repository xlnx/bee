#version 300 es

precision mediump float;

uniform sampler2D gImage;


in vec2 Position0;

out vec4 FragColor;

void main()
{
	vec2 uv = Position0;
	FragColor = texture(gImage, (uv + 1.) * .5);
}