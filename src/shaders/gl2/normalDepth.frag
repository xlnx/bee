#version 300 es

precision mediump float;

uniform mat4 gV;


in vec3 Normal0;

out vec4 FragColor;

vec2 normfloat2vec2(float x)
{
	const vec2 bitShift = vec2(1.0, 256.0);
	const vec2 bitMask = vec2(1.0/256.0, 0.0);
	vec2 rg = fract(x * bitShift);
	rg -= rg.gg * bitMask;
	return rg;
}

vec2 rgb2rgb565(vec3 rgb) {
	float g1 = rgb.g / 32.;
	float g2 = fract(rgb.g * 8.);
	g2 -= fract(g2 * 8.) / 8.;
	return vec2(rgb.r - fract(rgb.r * 32.) / 32. + g1, g2 + rgb.b/8.);
}

void main()
{
	FragColor = vec4(
		rgb2rgb565(normalize((gV * vec4(Normal0, 0)).xyz) * .5 + .5), 
		normfloat2vec2(gl_FragCoord.z)
	);
}