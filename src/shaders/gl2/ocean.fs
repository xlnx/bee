#version 300 es

precision mediump float;

uniform vec3 gCameraWorldPos;

// materials
uniform samplerCube gAmbient;
uniform float gSpecularIntensity;
uniform float gSpecularPower;



smooth in vec3 WorldPos0;
smooth in vec3 Normal0;

out vec4 FragColor;

void main()
{
	vec3 Normal = normalize(Normal0);
	vec3 VertexToEye = gCameraWorldPos - WorldPos0;
	vec3 R = reflect(-VertexToEye, Normal);

	FragColor = texture(gAmbient, R);
}