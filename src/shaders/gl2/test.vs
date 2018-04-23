#version 300 es

layout (location = 0) in lowp vec3 Position;
layout (location = 5) in lowp vec2 TexCoord;

out lowp vec2 TexCoord0;

uniform mat4 gWVP;

void main()
{
	gl_Position = gWVP * vec4(Position, 1);
	TexCoord0 = TexCoord;
}
