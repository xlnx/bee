#version 300 es

precision mediump float;

uniform sampler2D gPrevH;
uniform sampler2D gPrevDx;
uniform sampler2D gPrevDy;
uniform float gN;

// layout (location = 0) out vec2 H;
// layout (location = 1) out vec2 Dx;
// layout (location = 2) out vec2 Dy;
layout (location = 0) out vec3 Displacement;

void main()
{
	vec2 mn = floor(gl_FragCoord.xy);
	float term = 1. - 2. * mod(mn.x + mn.y, 2.);
	vec2 tex = mn / gN;
	float H = term * texture(gPrevH, tex).x;
	float Dx = term * texture(gPrevDx, tex).x;
	float Dy = term * texture(gPrevDy, tex).x;
	Displacement = vec3(Dx, Dy, H) * 3e-5;
}
