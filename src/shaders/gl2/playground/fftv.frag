#version 300 es

precision mediump float;

#define PI 3.141592654

uniform sampler2D gPrevH;
uniform sampler2D gPrevDx;
uniform sampler2D gPrevDy;
uniform float gStep;
uniform float gN;

layout (location = 0) out vec2 H;
layout (location = 1) out vec2 Dx;
layout (location = 2) out vec2 Dy;

vec2 complexMul(vec2 a, vec2 b)
{
	return vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x);
}

void main()
{
	vec2 mn = floor(gl_FragCoord.xy);
	float k = mod(mn.y, gStep * 2.);
	float theta = PI * k / gStep;
	vec2 eo = vec2(mn.y, mn.y + gStep) - step(gStep, k) * gStep;
	vec2 epos = vec2(mn.x, eo.x) / gN;
	vec2 opos = vec2(mn.x, eo.y) / gN;
	vec2 term = vec2(cos(theta), sin(theta));
	H = texture(gPrevH, epos).rg + complexMul(texture(gPrevH, opos).rg, term);
	Dx = texture(gPrevDx, epos).rg + complexMul(texture(gPrevDx, opos).rg, term);
	Dy = texture(gPrevDy, epos).rg + complexMul(texture(gPrevDy, opos).rg, term);
}
