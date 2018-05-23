#version 300 es

#define PI 3.141592654

precision mediump float;

uniform sampler2D gDisplacement;
uniform float gN;

in vec2 Position0;

out vec4 NormalJ;

const float ampl = 3e-5;
const float range = 1.5;
const float amplitude = 2e-3;
const float steepness = 1.;

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
	return x * vec2(cos(y), sin(y)) * .5 + .2; 
}

void main()
{
	vec2 tex = Position0 * .5 + .5;
	vec4 tdxy = vec4(1, 0, 0, 1) / gN;
	
	vec3 D = texture(gDisplacement, tex).xyz;
	vec3 Dx = texture(gDisplacement, tex + tdxy.xy).xyz - D;
	vec3 Dy = texture(gDisplacement, tex + tdxy.zw).xyz - D;
	
	vec3 d = D * ampl;
	vec3 dx = Dx * ampl;
	vec3 dy = Dy * ampl;

	vec3 n = cross(dx + tdxy.xyy * 2. * range, dy + tdxy.zwz * 2. * range);
	
	vec4 dxdy = vec4(Dx.xy, Dy.xy) * amplitude + tdxy;
	float jacobian = (1. + dxdy.x) * (1. + dxdy.w) - dxdy.y * dxdy.z;

	// float t = gTime;
	// vec2 seed = vec2(t, t * .5);
	// vec2 noise = .3 * gaussian(uv, seed);
	float turb = max(0., 1. - jacobian - .17);
		// length(noise));

	NormalJ = vec4(1e4 * n, smoothstep(0., 1., turb * steepness));
}
