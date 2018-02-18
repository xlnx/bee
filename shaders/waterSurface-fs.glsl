#version 330 core

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;
in vec3 Offset0;
// in vec4 Color0;

out vec4 FragColor;

struct Material
{
	sampler2D Diffuse;
};

struct LightBase
{
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct DirectionalLight
{
	LightBase Base;
	vec3 Direction;
};

struct PointLight
{
	LightBase Base;
	vec3 Position;
	float AttenConstant;
	float AttenLinear;
	float AttenExp;
};

uniform Material gMaterial;
uniform DirectionalLight gDirectionalLight;
uniform PointLight gPointLight[128];

uniform int gPointLightCount;
uniform vec3 gCameraWorldPos;
uniform float gMatSpecularIntensity;
uniform float gSpecularPower;

vec4 CalcLightInternal(LightBase Light, vec3 LightDirection, vec3 Normal)
{
	vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
	float DiffuseFactor = dot(Normal, -LightDirection);
	
	vec4 DiffuseColor = vec4(0, 0, 0, 0);
	vec4 SpecularColor = vec4(0, 0, 0, 0);

	if (DiffuseFactor > 0)
	{
		DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0f);

		vec3 VertexToEye = normalize(gCameraWorldPos - WorldPos0);
		vec3 LightReflect = normalize(reflect(LightDirection, Normal));
		float SpecularFactor = dot(VertexToEye, LightReflect);
		if (SpecularFactor > 0)
		{
			SpecularFactor = pow(SpecularFactor, gSpecularPower);
			SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
		}
	}
	return AmbientColor + DiffuseColor + SpecularColor;
}

vec4 CalcDirectionalLight(vec3 Normal)
{
	return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal);
}

vec4 CalcPointLight(int Index, vec3 Normal)
{
	vec3 LightDirection = WorldPos0 - gPointLight[Index].Position;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	vec4 Color = CalcLightInternal(gPointLight[Index].Base, LightDirection, Normal);
	float Attenuation = gPointLight[Index].AttenConstant + 
		gPointLight[Index].AttenLinear * Distance +
		gPointLight[Index].AttenExp * Distance * Distance;
	return Color / Attenuation;
}

void main()
{
	vec3 Normal = normalize(Normal0);
	vec4 Light = CalcDirectionalLight(Normal);

	for (int i = 0; i < gPointLightCount; ++i)
	{
		Light += CalcPointLight(i, Normal);
	}

	// FragColor = // texture2D(gMaterial.Diffuse, TexCoord0.xy)
	// 	vec4(0.5, 0.5, 0.5, 1) * Light;
	FragColor = vec4(0, 0.42, 0.6, 1)
		/*texture2D(gMaterial.Diffuse, TexCoord0.xy)*/ * Light;
}