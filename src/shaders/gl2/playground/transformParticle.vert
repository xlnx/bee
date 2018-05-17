#version 300 es

in vec3 Position;
in vec3 Color;

out vec3 Position_next;
out vec3 Color_next;

void main()
{
	gl_PointSize = 10.;
	gl_Position = vec4(Position.xy, 0, 1);
	Position_next = Position + vec3(.01, .01, 0);
	Color_next = Color + vec3(-.01, -.01, .01);
}