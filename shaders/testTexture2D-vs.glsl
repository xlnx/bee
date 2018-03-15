#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 2) in vec3 Normal;
layout (location = 5) in vec2 TexCoord;

uniform mat4 gWVP;
uniform mat4 gWorld;

out vec3 WorldPos0;

void main()
{
	vec4 Pos4 = vec4(Position, 1.0);
	gl_Position = gWVP * Pos4;
	WorldPos0 = (gWorld * Pos4).xyz;
}