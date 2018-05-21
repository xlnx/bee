#version 300 es

precision mediump float;

// materials
uniform sampler2D gMatDiffuse;
uniform float gSpecularIntensity;
uniform float gSpecularPower;


in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 NormalDepth;
layout (location = 2) out vec4 PositionType;

void main()
{
	FragColor = texture(gMatDiffuse, TexCoord0.xy);
	NormalDepth = vec4(Normal0, gl_FragCoord.z);
	PositionType = vec4(WorldPos0, 1.); // vessel
}