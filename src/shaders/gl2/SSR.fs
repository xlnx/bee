#version 300 es

precision mediump float;

uniform sampler2D gImage;
uniform sampler2D gNormalDepth;


in vec2 Position0;

out vec4 FragColor;

struct PointInfo
{
	vec3 normal;
	float depth;
};

float vec22normfloat(vec2 v)
{
	const vec2 bitShift = vec2(1.0, 1.0/256.0);
	return dot(v, bitShift);
}

vec3 rgb5652rgb(vec2 rgb565)
{
	float rr = fract(rgb565.r * 32.);
	float bb = fract(rgb565.g * 8.);
	return vec3(rgb565.r - rr / 32., rr + rgb565.g / 8. - bb / 64., bb);
}

PointInfo decodePoint(vec2 uv)
{
	PointInfo info;
	vec4 tex = texture(gNormalDepth, (uv + 1.) * .5);
	info.normal = rgb5652rgb(tex.rg) * 2. - 1.;
	info.depth = vec22normfloat(tex.ba);
	return info;
}

void main()
{
	vec2 uv = Position0;
	if (uv.x > 0.) {
		PointInfo pinfo = decodePoint(uv);
		FragColor = vec4(pinfo.normal, 1);
	} else {
		FragColor = texture(gImage, (uv + 1.) * .5);
	}
}