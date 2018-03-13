#version 330 core

in vec3 WorldPos0;

uniform vec3 gLightWorldPos;

out float FragColor;

void main()
{
	vec3 LightToVertex = WorldPos0 - gLightWorldPos;
	float LightToPixelDistance = length(LightToVertex);
	FragColor = LightToPixelDistance;
}