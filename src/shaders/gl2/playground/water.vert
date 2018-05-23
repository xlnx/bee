#version 300 es

uniform sampler2D gDisplacement;
uniform mat4 gWVP;
uniform mat4 gWorld;
uniform float gN;


in vec2 Position;

out vec2 Position0;
out vec3 WorldPos0;
out vec3 Normal0;

void main()
{
	vec2 uv = Position;
	vec2 tex = uv * .5 + .5;
	vec3 d = texture(gDisplacement, tex).xyz * .1;
	vec3 dx = texture(gDisplacement, tex + vec2(1, 0) / gN).xyz * .1 - d;
	vec3 dy = texture(gDisplacement, tex + vec2(0, 1) / gN).xyz * .1 - d;
	vec3 p = vec3(uv, 0.) + d;
	vec3 n0 = cross(dx + vec3(2, 0, 0) / gN, dy + vec3(0, 2, 0) / gN);
	gl_Position = gWVP * vec4(p, 1);
	Position0 = uv;
	WorldPos0 = (gWorld * vec4(p, 1)).xyz;
	Normal0 = (gWorld * vec4(n0, 0)).xyz;
}