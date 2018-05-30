#version 300 es

in vec2 Position;

out vec2 Position0;

void main()
{
	gl_Position = vec4(Position, 0, 1);
	Position0 = Position;
}