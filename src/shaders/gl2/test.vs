#version 300 es

layout (location = 0) in lowp vec2 Position;

uniform mat4 gWVP;

void main()
{
	gl_Position = gWVP * vec4(Position, 0, 1);
}
