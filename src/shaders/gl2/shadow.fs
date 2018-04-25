#version 300 es

precision mediump float;

uniform vec3 gLightWorldPos;


in vec3 WorldPos0;

out float FragColor;

void main()
{
	vec3 LightToVertex = WorldPos0 - gLightWorldPos;
	float LightToPixelDistance = length(LightToVertex);
	FragColor = LightToPixelDistance;
}