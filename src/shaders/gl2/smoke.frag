#version 300 es

precision mediump float;

uniform sampler2D gSmoke;

out vec4 FragColor;

void main()
{
	FragColor = texture(gSmoke, gl_PointCoord.xy);
	//vec4(1.);
}