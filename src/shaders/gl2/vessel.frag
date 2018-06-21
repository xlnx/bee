#version 300 es

precision mediump float;

// materials
uniform sampler2D gMatDiffuse;


in vec2 TexCoord0;
in vec3 WorldPos0;
in vec3 Normal0;
in float Depth0;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 NormalType;
layout (location = 2) out vec4 Extra;

void main()
{
	FragColor = vec4(texture(gMatDiffuse, TexCoord0).xyz, Depth0);
	NormalType = vec4(Normal0, 1.); // vessel
	Extra = vec4(WorldPos0, WorldPos0.z < 0. ? 0. : 1.);
}