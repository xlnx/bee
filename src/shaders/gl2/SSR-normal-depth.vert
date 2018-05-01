#version 300 es

uniform mat4 gIP;


in vec2 Position;

out vec2 Position0;
out vec3 Incidence0;

void main()
{
	gl_Position = vec4(Position, 0, 1);
	Position0 = Position;
	Incidence0 = (gIP * vec4(Position, 0, 1)).xyz;
}