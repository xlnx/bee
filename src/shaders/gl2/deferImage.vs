#version 300 es

in vec2 Position;
in vec2 TexCoord;

out vec2 TexCoord0;

void main()
{
	gl_Position = vec4(Position, 0, 1);
	TexCoord0 = TexCoord;
}