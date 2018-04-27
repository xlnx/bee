#version 300 es

uniform mat4 gWVP;
// uniform mat4 gVP;
uniform mat4 gWorld;
uniform mediump vec3 gCameraWorldPos;


in vec2 Position;

smooth out vec3 WorldPos0;
smooth out vec3 Normal0;

void main()
{
	vec4 Position0 = vec4(Position, 0, 1);
	gl_Position = gWVP * Position0;
	Normal0 = vec3(0, 0, 1);
	WorldPos0 = (gWorld * Position0).xyz;
}
