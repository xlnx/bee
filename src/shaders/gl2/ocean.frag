#version 300 es

precision mediump float;

uniform vec3 gCameraWorldPos;

// materials
uniform samplerCube gAmbient;
uniform float gSpecularIntensity;
uniform float gSpecularPower;

const float FresnelBias = 0.4;
const float FresnelPower = -1.;
const float FresnelScale = .7;



smooth in vec3 WorldPos0;
smooth in vec3 Normal0;

out vec4 FragColor;

void main()
{
	vec3 n = normalize(Normal0);
	vec3 ve = normalize(gCameraWorldPos - WorldPos0);

	if (gCameraWorldPos.z >= 0.) {
		vec3 r = reflect(-ve, n);
		vec3 t = refract(-ve, n, 1.0 / 1.33);

		float R = clamp(FresnelBias + FresnelScale * pow(1. + dot(r, n), FresnelPower), 0., 1.);

		FragColor = R * texture(gAmbient, r) + (1.0 - R) * texture(gAmbient, t);
	} else {
		n = -n;

		vec3 r = reflect(-ve, n);
		vec3 t = refract(-ve, n, 1.33 / 1.);
		
		float R = clamp(FresnelBias + FresnelScale * pow(1. + dot(r, n), FresnelPower), 0., 1.);

		FragColor = vec4(1.) * R;//R * texture(gAmbient, r) + (1.0 - R) * texture(gAmbient, t);
	}
}