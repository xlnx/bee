#version 300 es

precision mediump float;

uniform float gLifetime;
uniform sampler2D gSmoke;

in float Lifetime_next;

out vec4 FragColor;

void main()
{
	FragColor = vec4(vec3(0.), texture(gSmoke, gl_PointCoord.xy).a * .5 * 
			(cos(Lifetime_next - gLifetime / 2.) + 1.));
	//vec4(1.);
}