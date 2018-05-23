#version 300 es

precision mediump float;

uniform float gDt;
uniform float gMaxLifetime;

// in vec3 Position_next;
in float Lifetime_next;

out vec4 FragColor;

void main()
{
	if (Lifetime_next > 0.)
		FragColor = vec4(1.);
		// vec4(Lifetime_next / gMaxLifetime);
		// vec4(vec3(gl_PointCoord.xy, 0), 1);
}