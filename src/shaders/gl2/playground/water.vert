#version 300 es

uniform sampler2D gNormalHeight;
uniform mat4 gWVP;
uniform mat4 gWorld;


in vec2 Position;

out vec2 Position0;
out vec3 WorldPos0;
out vec3 Normal0;

void main()
{
	vec2 uv = Position;
	vec4 nh = texture(gNormalHeight, uv * .5 + .5);
	vec3 p = vec3(uv, nh.w);
	gl_Position = gWVP * vec4(p, 1);
	WorldPos0 = (gWorld * vec4(p, 1)).xyz;
	Position0 = uv;
	Normal0 = nh.xyz;
}