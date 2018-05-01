#version 300 es

precision mediump float;

uniform sampler2D gImage;
uniform sampler2D gNormal;
uniform sampler2D gDepth;
uniform mat4 gP;

uniform vec3 gCameraWorldPos;

#define RAYMARCH_MAX_ITER 16
#define RAYMARCH_ITER_STEP 1e-1
#define RAYMARCH_EPS 1.5e-2


in vec2 Position0;
in vec3 Incidence0;

out vec4 FragColor;

struct PointInfo
{
	vec3 normal;
	float depth;
};

float vec42normfloat(vec4 v)
{
	const vec4 bitShift = vec4(1.0, 1.0/256.0, 1.0/(256.0*256.0), 1.0/(256.0*256.0*256.0));
	return dot(v, bitShift);
}

float decodeDepth(float d)
{
	return sqrt(- log(d) - 1.);
}

float getPointDepth(vec2 uv)
{
	return decodeDepth(vec42normfloat(texture(gDepth, (uv + 1.) * .5)));
}

PointInfo decodePoint(vec2 uv)
{
	PointInfo info;
	info.normal = texture(gNormal, (uv + 1.) * .5).xyz * 2. - 1.;
	info.depth = getPointDepth(uv);
	// vec42normfloat(texture(gDepth, (uv + 1.) * .5));
	return info;
}

void main()
{
	vec2 uv = Position0;
	PointInfo pinfo = decodePoint(uv);
	vec3 N = pinfo.normal;
	vec3 I = Incidence0;
	// if (uv.x > 0.) {
		vec3 color = vec3(0.);
		float alpha = 0.;
		if (pinfo.depth != 0.) {
			vec3 SR = (gP * vec4(reflect(I, N), 0.)).xyz;
			vec2 SRP = normalize(SR.xy);
			vec3 step = vec3(SRP, SR.z * SRP.x / SR.x) * RAYMARCH_ITER_STEP;
			vec3 p = vec3(uv, pinfo.depth);
			for (int i = 0; i != RAYMARCH_MAX_ITER; ++i) {
				p += step;
				if (p.z > getPointDepth(p.xy)) {
					p -= step;
					step = step * .5;
				}
				if (p.x > 1. || p.y > 1. || p.x < -1. || p.x < -1.) {
					p.z = 1e7;
					break;
				}
			}
			float d = abs(p.z - getPointDepth(p.xy));
			if (d < RAYMARCH_EPS) {
				// float mp = cos(2. * d / RAYMARCH_EPS);
				color = texture(gImage, (p.xy + 1.) * .5).xyz;
				// alpha += mp * 0.25;
				alpha = 0.6 * cos(2. * d / RAYMARCH_EPS);
			} else {
				// test
				// if (p.z < getPointDepth(p.xy)) {
				// 	color = vec3(0.);
				// } else {
				// 	color = vec3(1.);
				// }
				// alpha = 1.;
			}
		}

		FragColor = //vec4(1.) * pinfo.depth;
			vec4(mix(texture(gImage, (uv + 1.) * .5).xyz, 
			clamp(color, 0., 1.), clamp(alpha, 0., 1.)), .5);//vec4(ray, 0, 1);
		// FragColor = texture(gImage, (uv + 1.) * .5) + vec4(color, 1);
	// } else {
	// 	FragColor = texture(gImage, (uv + 1.) * .5);
	// }
}