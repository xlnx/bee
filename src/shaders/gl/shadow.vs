#version 300 es

uniform mat4 gWVP;
uniform mat4 gWorld;


in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

out vec3 WorldPos0;

void main()
{
	vec4 Pos4 = vec4(Position, 1.0);
	gl_Position = gWVP * Pos4;
	WorldPos0 = (gWorld * Pos4).xyz;
}