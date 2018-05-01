#version 300 es

precision mediump float;

uniform sampler2D gMain;

in vec2 TexCoord0;

out vec4 FragColor;

void main()
{
	FragColor = texture(gMain, TexCoord0);
}