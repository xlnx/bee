#version 300 es

uniform mat4 gSpace;


in vec2 Position;

out vec3 WorldPos0;

void main()
{
	vec4 Pos = vec4(Position, 0, 1);
	gl_Position = Pos;
	WorldPos0 = normalize((gSpace * Pos).xyz);
}
