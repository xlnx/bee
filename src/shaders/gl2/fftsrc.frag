#version 300 es

#define PI 3.141592654
#define EPS .00

precision mediump float;

uniform sampler2D gSpectrum;
// uniform sampler2D gGaussian;
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

float UVRandom(vec2 uv, float salt, float random)
{
	uv += vec2(salt, random);
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 gaussian(vec2 uv, vec2 seed)
{
	float rand1 = UVRandom(uv, 10.612, seed.x);
	float rand2 = UVRandom(uv, 11.899, seed.y);
	float x = sqrt(2. * log(rand1 + 1.));
	float y = 2. * PI * rand2;
	return x * vec2(cos(y), sin(y)); 
}

void main()
{
	float t = gTime;
	vec2 uv = Position0;
	float k = length(uv);
	float sinv = sin(omega * t);
	float cosv = cos(omega * t);
	vec2 tex = uv * .5 + .5;
	vec2 seed = vec2(1.);//vec2(1., 2.) * t * 1e-6;
	vec2 h0 = texture(gSpectrum, tex).xy;
	vec2 H0 = complexMul(h0, gaussian(tex, seed * .5 + 1.));
	vec2 h0_conj = texture(gSpectrum, -tex).xy * vec2(1, -1);
	vec2 H0_conj = complexMul(h0_conj, gaussian(tex, seed));
	H = (H0 + H0_conj).xy * cosv + (H0 - H0_conj).yx * vec2(-1, 1) * sinv;
	Dx = (uv.x / k * H).yx * vec2(-1., 1.);
	Dy = (uv.y / k * H).yx * vec2(-1., 1.);
}