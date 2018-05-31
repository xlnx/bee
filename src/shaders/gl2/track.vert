#version 300 es

uniform mat4 gWorld;
uniform mat4 gV;
uniform mat4 gVP;
uniform float gDt;
uniform float gTime;
uniform mediump float gLifetime;
uniform vec3 gOrigin;
uniform vec3 gVelocity;
uniform sampler2D gGaussian;

in vec4 WorldPos;
in float Lifetime;
in vec3 Velocity;

out vec4 WorldPos_next;
out float Lifetime_next;
out vec3 Velocity_next;

const vec3 u = vec3(1, 0, 0);

void main()
{
	vec4 p = WorldPos;
	float t = Lifetime;
	if (Lifetime > gLifetime)
	{
		t -= gLifetime;
		p = gWorld * vec4(gOrigin, 1);
		vec3 n = gVelocity;
		vec2 rn = texture(gGaussian, sin(p.yx * 1027.) + vec2(1., .7) * gTime).xy;
		// Velocity_next = normalize(vec3(n.xy * .3, 1.));
		vec3 v = cross(u, n);
		Velocity_next = (v * rn.x + u * rn.y) + n;
	}
	else
	{
		Velocity_next = Velocity;
	}
	gl_Position = gVP * p;
	vec4 pv = gV * p;
	gl_PointSize = - 200. / pv.z;
	WorldPos_next = p + vec4(Velocity * gDt * .25, 0);
	Lifetime_next = t + gDt;
}
