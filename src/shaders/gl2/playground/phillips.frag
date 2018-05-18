#version 300 es

precision mediump float;

uniform sampler2D gGaussian;

in vec2 Position0;

out vec2 Phillips;

// wind speed 
const float V = 1e4;
// wind direction
const vec2 u = normalize(vec2(1, -1));
// gravity
const float g = 9.8;
// parameter
const float L = V * V / g;
// amplitude
const float A = 2.;

void main()
{
	vec2 uv = Position0;
	float k = length(uv);
	vec2 kn = normalize(uv);
	float d = dot(kn, u);
	vec2 gaussian = (texture(gGaussian, uv * .5 + .5) * 2. - 1.).xy;
	vec2 ph = vec2(A / pow(k, 4.) * d * d * exp(-1./(k * k * L * L)));
	Phillips = 1. / sqrt(2.) * gaussian * sqrt(ph);
}