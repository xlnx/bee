#version 300 es

// uniform mat4 gWVP;
uniform mat4 gVP;
uniform mat4 gWorld;


in vec2 Position;

smooth out vec3 WorldPos0;
smooth out vec3 Normal0;

void main()
{
	gl_Position = gVP * vec4(Position, 0, 1);
	Normal0 = vec3(0, 0, 1);
	WorldPos0 = (gWorld * vec4(Position, 0, 1.0)).xyz;
}
