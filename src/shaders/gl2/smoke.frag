#version 300 es

precision mediump float;

uniform float gLifetime;
uniform sampler2D gSmokeBall;

in float Lifetime_next;

out vec4 FragColor;

void main()
{
	float t = Lifetime_next / gLifetime;
	FragColor = vec4(vec3(0.), 
		texture(gSmokeBall, gl_PointCoord.xy).a * (sqrt(t) - t) * 3.);
}