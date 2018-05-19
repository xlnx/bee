#version 300 es

precision mediump float;

uniform sampler2D gImage;

in vec2 Position0;

out vec2 FragColor;

void main()
{
	FragColor = texture(gImage, Position0 * .5 + .5).xy * 2. - 1.;
}