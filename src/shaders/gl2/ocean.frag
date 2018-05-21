#version 300 es

precision mediump float;

// materials
uniform samplerCube gAmbient;
uniform float gSpecularIntensity;
uniform float gSpecularPower;


in vec3 WorldPos0;
in vec3 Normal0;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 NormalDepth;
layout (location = 2) out vec4 PositionType;

void main()
{
	vec3 n = normalize(Normal0);
	NormalDepth = vec4(n, gl_FragCoord.z);
	PositionType = vec4(WorldPos0, 0.); // sea
}