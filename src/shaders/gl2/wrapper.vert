#version 300 es

// camera
uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mat4 gWV;


in vec3 Position;

out float Depth0;

void main()
{
	gl_Position = gWVP * vec4(Position, 1);
	Depth0 = (gWV * vec4(Position, 1.0)).z;
}
