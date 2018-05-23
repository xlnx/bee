#version 300 es

uniform mediump float gDt;
uniform mediump float gMaxLifetime;
uniform vec2 gVelocity;
uniform vec2 gPosition;

in vec3 Position;
in float Lifetime;

out vec3 Position_next;
out float Lifetime_next;

void main()
{
	gl_PointSize = 50.;
	gl_Position = vec4(Position.xy, 0, 1);

	Lifetime_next = Lifetime + gDt;
	if (Lifetime_next > gMaxLifetime)
	{
		Lifetime_next -= gMaxLifetime;
		Position_next = vec3(gPosition, 0.);
	}
	else
	{
		Position_next = Position;// + Velocity_next * gDt;// * .2;
	}
}
