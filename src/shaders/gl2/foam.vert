#version 300 es

uniform mediump float gDt;
uniform mat4 gWorld;
uniform mat4 gVP;
uniform mediump float gTime;
uniform mediump float gLifetime;
uniform vec2 gOrigin;

in vec4 WorldPos;
in vec2 Offset;
in float Lifetime;

out vec4 WorldPos_next;
out vec2 Offset_next;
out float Lifetime_next;

out vec2 TexCoord0;

void main()
{
	vec4 p = WorldPos;
	float t = Lifetime;
	if (Lifetime > gLifetime)
	{
		t -= gLifetime;
		p = gWorld * vec4(gOrigin + Offset, 0.05, 1);
	}
	gl_Position = gVP * p;
	Offset_next = Offset;
	WorldPos_next = p;// + vec4(Velocity * gDt * .25, 0);
	Lifetime_next = t + gDt;
	TexCoord0 = vec2(step(0., Offset.x), 
		1. - (t + 1. - step(0., Offset.y)) / gLifetime);
}
