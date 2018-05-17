#version 300 es

precision mediump float;

uniform vec3 gLight;
uniform vec3 gCameraWorldPos;


in vec2 Position0;
in vec3 WorldPos0;
in vec3 Normal0;

out vec4 FragColor;

void main()
{
	vec2 uv = Position0;
	vec3 n = normalize(Normal0);
	FragColor = vec4(0.2, 0.3, 0.6, 1.) * 1.4 + 
		vec4(.3) * dot(reflect(normalize(gLight), n), normalize(gCameraWorldPos - WorldPos0));// vec4(n * .5 + .5, 1.);
}