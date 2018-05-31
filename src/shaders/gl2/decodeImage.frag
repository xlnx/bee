#version 300 es

precision mediump float;

uniform sampler2D gImage;

in vec2 Position0;

out vec4 FragColor;

void main()
{
	FragColor = texture(gImage, Position0 * .5 + .5) * .5 + .5;
}