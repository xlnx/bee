#version 300 es

uniform sampler2D gDisplacement;
uniform mat4 gWVP;
uniform mat4 gWorld;


in vec2 Position;

out vec2 Position0;
out vec3 WorldPos0;
out vec3 Normal0;

void main()
{
	vec2 uv = Position;
	vec3 diff = texture(gDisplacement, uv * .5 + .5).xyz;
	vec3 p = vec3(uv, 0.) + diff * .1;
	gl_Position = gWVP * vec4(p, 1);
	WorldPos0 = (gWorld * vec4(p, 1)).xyz;
	Position0 = uv;
	Normal0 = vec3(0, 0, 1);//nh.xyz;
}