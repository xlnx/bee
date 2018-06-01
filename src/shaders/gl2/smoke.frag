#version 300 es

precision mediump float;

uniform float gLifetime;
uniform sampler2D gSmoke;
uniform vec3 gColor;
uniform float gOpacity;

in float Lifetime_next;

out vec4 FragColor;

void main()
{
	float t = Lifetime_next / gLifetime;
	FragColor = vec4(gColor, 
		texture(gSmoke, gl_PointCoord.xy).a * (sqrt(t) - t) / gOpacity);
}