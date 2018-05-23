#version 300 es

precision mediump float;

uniform vec3 gLight;
uniform vec3 gCameraWorldPos;
uniform sampler2D gWhitecap;

in vec2 Position0;
in vec3 WorldPos0;
in vec3 Normal0;

out vec4 FragColor;

void main()
{
	vec2 uv = Position0;
	vec3 n = normalize(Normal0);
	FragColor = vec4(texture(gWhitecap, uv * .5 + .5).r);
	(
		vec4(0.2, 0.3, 0.7, 1.) + 
		vec4(.15) * dot(reflect(normalize(gLight), n), 
			normalize(gCameraWorldPos - WorldPos0))
	);
}