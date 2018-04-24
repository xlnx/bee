#version 300 es

uniform mat4 gWVP;
uniform mat4 gWorld;


layout (location = 0) in vec3 Position;
layout (location = 2) in vec3 Normal;
layout (location = 5) in vec2 TexCoord;

out vec2 TexCoord0;
out vec3 WorldPos0;
out vec3 Normal0;

void main()
{
	gl_Position = gWVP * vec4(Position, 1);
	TexCoord0 = TexCoord;
	Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;
	WorldPos0 = (gWorld * vec4(Position, 1.0)).xyz;
}
