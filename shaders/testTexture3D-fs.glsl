#version 330 core

in vec3 TexVector0;

out vec4 FragColor;

uniform samplerCube gSampler;

void main()
{
	FragColor = vec4(vec3(texture(gSampler, TexVector0).r-1e24), 1);
	// FragColor = vec4(TexVector0, 1);
}