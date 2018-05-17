#version 300 es

uniform mat4 gWVP;
uniform mat4 gWorld;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

out vec3 Normal0;
out vec2 Position0;
out vec2 TexCoord0;
out vec3 WorldPos0;

void main()
{
	gl_Position = gWVP * vec4(Position, 1);
	Normal0 = (gWorld * vec4(Normal, 0)).xyz;
	Position0 = Position.xy;
	WorldPos0 = (gWorld * vec4(Position, 1)).xyz;
	TexCoord0 = TexCoord;
}