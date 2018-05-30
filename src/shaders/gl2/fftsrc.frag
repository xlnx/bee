#version 300 es

precision mediump float;

uniform sampler2D gSpectrum;
uniform sampler2D gGaussian;
uniform float gTime;

in vec2 Position0;

layout (location = 0) out vec2 H;
layout (location = 1) out vec2 Dx;
layout (location = 2) out vec2 Dy;

const float omega = 2.;

vec2 complexMul(vec2 a, vec2 b)
{
	return vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x);
}

void main()
{
	float t = gTime;
	vec2 uv = Position0;
	float k = length(uv);
	float sinv = sin(omega * t);
	float cosv = cos(omega * t);
	vec2 tex = uv * .5 + .5;
	vec2 seed = vec2(1.);
	vec4 xi = texture(gGaussian, tex);
		// vec2(1., 2.) * t * 1e-7;
	vec2 h0 = texture(gSpectrum, tex).xy;
	vec2 H0 = complexMul(h0, xi.xy);
	vec2 h0_conj = texture(gSpectrum, -tex).xy * vec2(1, -1);
	vec2 H0_conj = complexMul(h0_conj, xi.zw);
	H = (H0 + H0_conj).xy * cosv + (H0 - H0_conj).yx * vec2(-1, 1) * sinv;
	Dx = (uv.x / k * H).yx * vec2(-1., 1.);
	Dy = (uv.y / k * H).yx * vec2(-1., 1.);
}