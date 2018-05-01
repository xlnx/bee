#version 300 es

precision mediump float;

// camera
uniform vec3 gCameraWorldPos;

// materials
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
uniform sampler2D gAmbient;
uniform sampler2D gEmissive;
uniform sampler2D gNormals;
uniform float gSpecularIntensity;
uniform float gSpecularPower;

// pointlight
#define MAX_POINT_LIGHTS 24
uniform vec3 gPointLight_color[MAX_POINT_LIGHTS];
uniform float gPointLight_ambientIntensity[MAX_POINT_LIGHTS];
uniform float gPointLight_diffuseIntensity[MAX_POINT_LIGHTS];
uniform vec3 gPointLight_position[MAX_POINT_LIGHTS];
uniform float gPointLight_attenConstant[MAX_POINT_LIGHTS];
uniform float gPointLight_attenLinear[MAX_POINT_LIGHTS];
uniform float gPointLight_attenExp[MAX_POINT_LIGHTS];
uniform int gPointLight_count;


varying vec2 TexCoord0;
varying vec3 WorldPos0;
varying vec3 Normal0;

void CalcLightInternal(vec3 Light_color, float Light_ambientIntensity,
	float Light_diffuseIntensity, vec3 LightDirection, vec3 Normal, 
	float ShadowFactor, out vec3 Light0, out vec3 LightSpec0)
{
	vec3 AmbientColor = vec3(Light_color * Light_ambientIntensity);
	float DiffuseFactor = dot(Normal, -LightDirection);
	
	vec3 DiffuseColor = vec3(0, 0, 0);
	vec3 SpecularColor = vec3(0, 0, 0);

	if (DiffuseFactor > 0.0)
	{
		DiffuseColor = vec3(Light_color * Light_diffuseIntensity * DiffuseFactor);

		vec3 VertexToEye = normalize(gCameraWorldPos - WorldPos0);
		vec3 LightReflect = normalize(reflect(LightDirection, Normal));
		float SpecularFactor = dot(VertexToEye, LightReflect);
		if (SpecularFactor > 0.0)
		{
			SpecularFactor = pow(SpecularFactor, gSpecularPower);
			SpecularColor = SpecularFactor * gSpecularIntensity * Light_color;
		}
	}
	Light0 = AmbientColor + ShadowFactor * DiffuseColor;
	LightSpec0 = step(1.0, ShadowFactor) * SpecularColor;
}

void main()
{
	vec3 Normal = normalize(Normal0);
	float ShadowFactor = 1.0;
	// float ShadowFactor = CalcShadowFactor(WorldPos0 - gLightWorldPos);
	vec3 Light = vec3(0, 0, 0), LightSpec = vec3(0, 0, 0);

	for (int i = 0; i < MAX_POINT_LIGHTS; ++i) if (i < gPointLight_count)
	{
		// CalcPointLight(i, Normal, ShadowFactor, Light, LightSpec);
		vec3 LightDirection = WorldPos0 - gPointLight_position[i];
		float Distance = length(LightDirection);
		LightDirection = normalize(LightDirection);

		vec3 Light0, LightSpec0;
		CalcLightInternal(gPointLight_color[i], gPointLight_ambientIntensity[i], 
			gPointLight_diffuseIntensity[i], LightDirection, Normal, 
			ShadowFactor, Light0, LightSpec0);
		float Attenuation = gPointLight_attenConstant[i] + 
			gPointLight_attenLinear[i] * Distance +
			gPointLight_attenExp[i] * Distance * Distance;
		Light += Light0 / Attenuation; LightSpec += LightSpec0;
	} else break;

	gl_FragColor = texture2D(gDiffuse, TexCoord0.xy) * vec4(Light, 1.0) + vec4(LightSpec, 1.0);
}
