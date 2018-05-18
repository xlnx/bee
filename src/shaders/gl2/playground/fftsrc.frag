#version 300 es

precision mediump float;

uniform sampler2D gSpectrum;
uniform float gTime;

in vec2 Position0;

layout (location = 0) out vec2 H;
layout (location = 1) out vec2 Dx;
layout (location = 2) out vec2 Dy;

const float omega = .5;

void main()
{
	float t = gTime;
	vec2 uv = Position0;
	float k = length(uv);
	float sinv = sin(omega * t);
	float cosv = cos(omega * t);
	vec2 H0 = texture(gSpectrum, uv *.5 + .5).xy;
	vec2 H0_conj = texture(gSpectrum, -uv * .5 + .5).xy * vec2(1, -1);
	H = (H0 + H0_conj).xy * cosv + (H0 - H0_conj).yx * vec2(-1, 1) * sinv;
	Dx = (uv.x / k * H).yx * vec2(-1., 1.);
	Dy = (uv.y / k * H).yx * vec2(-1., 1.);
}