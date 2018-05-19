#version 300 es

precision mediump float;

#define PI 3.141592654

uniform sampler2D gPrevStep;
uniform float gStep;
uniform float gN;

out vec2 Step;

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
	vec2 e = texture(gPrevStep, vec2(mn.x, eo.x) / gN).rg;
	vec2 o = texture(gPrevStep, vec2(mn.x, eo.y) / gN).rg;
	Step = e + complexMul(o, vec2(cos(theta), sin(theta)));
}
