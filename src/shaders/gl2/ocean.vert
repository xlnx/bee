#version 300 es

uniform sampler2D gDisplacement;
uniform mat4 gWVP;
uniform mat4 gWorld;
uniform float gN;
uniform mediump vec3 gCameraWorldPos;


in vec2 Position;

out vec2 Position0;
out vec3 WorldPos0;
out vec3 Normal0;

const float scale = .8;

void main()
{
	vec2 uv = Position + gCameraWorldPos.xy;
	vec2 tex = uv / 40. * .5 + .5;
	vec3 d = texture(gDisplacement, tex).xyz * scale;
	vec3 dx = texture(gDisplacement, tex + vec2(1, 0) / gN).xyz * scale - d;
	vec3 dy = texture(gDisplacement, tex + vec2(0, 1) / gN).xyz * scale - d;
	vec3 p = vec3(uv, 0.) + d;
	vec3 n0 = cross(
		normalize(dx + vec3(20, 0, 0) / gN), 
		normalize(dy + vec3(0, 20, 0) / gN)
	);
	gl_Position = gWVP * vec4(p, 1);
	WorldPos0 = (gWorld * vec4(p, 1)).xyz;
	// Normal0 = vec3(0, 0, 1);
	Normal0 = normalize(gWorld * vec4(n0, 0)).xyz;
}
