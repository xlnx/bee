#version 300 es

uniform mat4 gWorld;
uniform mediump mat4 gV;
uniform mat4 gVP;
uniform float gDt;

uniform mediump float gLifetime;
uniform vec3 gOrigin;
uniform float gSpeed;
uniform sampler2D gGaussian;

in vec4 WorldPos;
in float Lifetime;
in vec3 Velocity;
in float Acceleration;
in float GroupId;
in float InstanceId;
in float StartTime;

out vec4 WorldPos_next;
out float Lifetime_next;
out vec3 Velocity_next;
out float Acceleration_next;
out float GroupId_next;
out float InstanceId_next;
out float StartTime_next;

const vec3 g = vec3(0, 0, -9.8 * .1 * .5);

void main()
{
	vec4 p = WorldPos;
	float t = Lifetime;
	if (Lifetime > gLifetime)
	{
		t -= gLifetime;
		p = gWorld * vec4(gOrigin, 1);
		vec4 gauss = texture(gGaussian, gOrigin.xy + .1 * vec2(GroupId + StartTime));
		vec3 v0 = vec3(gauss.xy * .9, gSpeed * (gauss.z * .5 + 1.));
		float l = (10. - InstanceId) / 10. * .50;
		Velocity_next = v0 * l;
		Acceleration_next = l * l;
	}
	else
	{
		Velocity_next = Velocity + g * Acceleration * gDt;
		Acceleration_next = Acceleration;
	}
	gl_Position = gVP * p;
	vec4 pv = gV * p;
	gl_PointSize = - 50. * sqrt(60. * InstanceId + 1.) / pv.z;
	WorldPos_next = p + vec4(Velocity * gDt, 0);
	Lifetime_next = t + gDt;
	GroupId_next = GroupId;
	InstanceId_next = InstanceId;
	StartTime_next = StartTime;
}
