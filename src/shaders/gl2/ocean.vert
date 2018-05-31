#version 300 es

uniform sampler2D gDisplacement;
uniform sampler2D gNormalJ;

uniform mat4 gWVP;
uniform mediump mat4 gWorld;
uniform mediump vec3 gCameraWorldPos;


in vec2 Position;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;
out float Distance0;

const float range = 1.5;
const float amplitude = 3e-5;
// 2.5;

void main()
{
	vec2 uv = Position + gCameraWorldPos.xy;
	TexCoord0 = uv / range * .5 + .5;
	vec3 d = texture(gDisplacement, TexCoord0).xyz * amplitude;
	vec3 p = vec3(uv, 0.) + d;
	gl_Position = gWVP * vec4(p, 1);
	WorldPos0 = (gWorld * vec4(p, 1)).xyz;
	Distance0 = length(Position);
}
