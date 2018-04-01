#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 2) in vec3 Normal;
layout (location = 5) in vec2 TexCoord;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;
// out vec4 Color0;

uniform mat4 gWVP;
uniform mat4 gWorld;

void main()
{
	vec3 diff = normalize(Normal) * 2;
	gl_Position = gWVP * vec4(Position + diff, 1.0);
	TexCoord0 = TexCoord;
	Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;
	WorldPos0 = (gWorld * vec4(Position + diff, 1.0)).xyz;
	// Color0 = vec4(clamp(Position, 0.0, 1.0), 1.0);
}