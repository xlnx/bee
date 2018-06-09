#version 300 es

uniform mediump vec3 gCameraWorldPos;
uniform mat4 gWVP;


in vec3 Position;

out vec3 WorldPos0;

void main()
{
	gl_Position = gWVP * vec4(Position + gCameraWorldPos, 1);
	WorldPos0 = Position;
}