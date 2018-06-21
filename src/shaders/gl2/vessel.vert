#version 300 es

// camera
uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mat4 gWV;


in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

out vec2 TexCoord0;
out vec3 WorldPos0;
out vec3 Normal0;
out float Depth0;

void main()
{
	gl_Position = gWVP * vec4(Position, 1);
	Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;
	WorldPos0 = (gWorld * vec4(Position, 1)).xyz;
	Depth0 = (gWV * vec4(Position, 1.0)).z;
	TexCoord0 = TexCoord;
}
