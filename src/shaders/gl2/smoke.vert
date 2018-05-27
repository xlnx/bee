#version 300 es

uniform mat4 gWorld;
uniform mat4 gV;
uniform mat4 gVP;
uniform float gDt;
uniform mediump float gLifetime;
uniform vec3 gOrigin;

in vec4 WorldPos;
in float Lifetime;

out vec4 WorldPos_next;
out float Lifetime_next;

void main()
{
	vec4 p = WorldPos;
	float t = Lifetime;
	if (Lifetime > gLifetime)
	{
		t -= gLifetime;
		p = gWorld * vec4(gOrigin, 1);
	}
	gl_Position = gVP * p;
	vec4 pv = gV * p;
	gl_PointSize = - 200. / pv.z;
	WorldPos_next = p + vec4(vec3(0., 0., 1.) * gDt * .25, 0);
	Lifetime_next = t + gDt;
}
