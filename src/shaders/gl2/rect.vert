#version 300 es

uniform vec2 gDisplacement;

in vec2 Position;
in vec2 TexCoord;

out vec2 TexCoord0;

void main()
{
	gl_Position = vec4(Position + gDisplacement, 0, 1);
	TexCoord0 = TexCoord;
}