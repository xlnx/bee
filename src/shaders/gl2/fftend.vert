#version 300 es

in vec2 Position;

void main()
{
	gl_Position = vec4(Position, 0, 1);
}