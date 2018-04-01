#version 330 core

uniform float gObjectIndex;

out float FragColor;

void main()
{
	FragColor = gObjectIndex;
}