#version 300 es

precision mediump float;

uniform sampler2D gImage;
uniform sampler2D gNormalDepth;
uniform mat4 gP;

uniform vec3 gCameraWorldPos;

#define RAYMARCH_MAX_ITER 16
#define RAYMARCH_ITER_STEP 0.02
#define RAYMARCH_EPS 2e-2


in vec2 Position0;
in vec3 Incidence0;

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

float getPointDepth(vec2 uv)
{
	return vec22normfloat(texture(gNormalDepth, (uv + 1.) * .5).ba);
}

void main()
{
	vec2 uv = Position0;
	PointInfo pinfo = decodePoint(uv);
	vec3 N = pinfo.normal;
	vec3 I = Incidence0;
	if (uv.x > 0.) {
		vec3 color = texture(gImage, (uv + 1.) * .5).xyz;
		if (pinfo.depth != 0.) {
			vec3 R = reflect(I, N);
			vec3 ray = normalize((gP * vec4(R, 0)).xyz) * RAYMARCH_ITER_STEP;
			vec3 p = vec3(uv, pinfo.depth);
			for (int i = 0; i != RAYMARCH_MAX_ITER; ++i) {
				p += ray;
				float d = abs(p.z - getPointDepth(p.xy));
				if (d < RAYMARCH_EPS) {
					float mp = cos(2. * d / RAYMARCH_EPS);
					color += texture(gImage, (p.xy + 1.) * .5).xyz * mp * 0.7;
				}
			}
		}

		FragColor = vec4(color, 1);//vec4(ray, 0, 1);
		// FragColor = texture(gImage, (uv + 1.) * .5) + vec4(color, 1);
	} else {
		FragColor = texture(gImage, (uv + 1.) * .5);
	}
}