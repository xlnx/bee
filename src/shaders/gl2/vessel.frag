#version 300 es

precision mediump float;

// materials
uniform sampler2D gMatDiffuse;


in vec2 TexCoord0;
in vec3 Normal0;
in float Depth0;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 NormalType;

void main()
{
	FragColor = vec4(texture(gMatDiffuse, TexCoord0).xyz, Depth0);
	NormalType = vec4(Normal0, 1.); // vessel
}