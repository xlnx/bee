#version 330 core

out vec4 FragColor;

uniform vec3 gColor;

void main()
{
	FragColor = vec4(gColor, 1);//vec4(0, 0, 1, 1);
}