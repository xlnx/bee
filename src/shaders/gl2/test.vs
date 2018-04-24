#version 300 es

uniform mat4 gWVP;


layout (location = 0) in vec3 Position;
layout (location = 5) in vec2 TexCoord;

out vec2 TexCoord0;

void main()
{
	gl_Position = gWVP * vec4(Position, 1);
	TexCoord0 = TexCoord;
}
