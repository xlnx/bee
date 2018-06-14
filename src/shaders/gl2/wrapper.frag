#version 300 es

precision mediump float;

in float Depth0;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 NormalType;
layout (location = 2) out vec4 Extra;

void main()
{
	FragColor = vec4(1, 0, 0, Depth0);
	NormalType = vec4(0, 0, 1, 1.); // vessel
	Extra.w = 1.;
}