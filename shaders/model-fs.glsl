#version 330 core

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;
// in vec4 Color0;

out vec4 FragColor;

struct LightBase
{
	vec3 Color;
	float AmbientIntensity;
	float Intensity;
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

uniform struct
{
	DirectionalLight Instance[1];
	int Count;
} gDirectionalLight;

uniform struct
{
	PointLight Instance[128];
	int Count;
} gPointLight;

uniform struct
{
	sampler2D Diffuse;
	sampler2D Specular;
	sampler2D Ambient;
	sampler2D Emissive;
	sampler2D Normals;

	float SpecularIntensity;
	float SpecularPower;

	float DiffuseIntensity;
} gMaterial;

uniform vec3 gCameraWorldPos;
uniform vec3 gLightWorldPos;
uniform samplerCube gShadowMap;

float CalcShadowFactor(vec3 LightDirection)
{
	float SampledDistance = texture(gShadowMap, LightDirection).r;
	float Distance = length(LightDirection);
	if (Distance < SampledDistance + 1e-1)
		return 1.0;
	else
		return 0.4;
}

vec4 CalcLightInternal(LightBase Light, vec3 LightDirection, vec3 Normal, float ShadowFactor)
{
	vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
	float DiffuseFactor = dot(Normal, -LightDirection);
	
	vec4 DiffuseColor = vec4(0, 0, 0, 0);
	vec4 SpecularColor = vec4(0, 0, 0, 0);

	if (DiffuseFactor > 0)
	{
		DiffuseColor = vec4(Light.Color * gMaterial.DiffuseIntensity * DiffuseFactor, 1.0f);

		vec3 VertexToEye = normalize(gCameraWorldPos - WorldPos0);
		vec3 LightReflect = normalize(reflect(LightDirection, Normal));
		float SpecularFactor = dot(VertexToEye, LightReflect);
		if (SpecularFactor > 0)
		{
			SpecularFactor = pow(SpecularFactor, gMaterial.SpecularPower);
			SpecularColor = vec4(SpecularFactor * gMaterial.SpecularIntensity * Light.Color, 1.0f);
		}
	}
	return (AmbientColor + ShadowFactor * (DiffuseColor + SpecularColor)) * Light.Intensity;
}

vec4 CalcDirectionalLight(int Index, vec3 Normal)
{
	return CalcLightInternal(gDirectionalLight.Instance[Index].Base, 
		gDirectionalLight.Instance[Index].Direction, Normal, 1.0);
}

vec4 CalcPointLight(int Index, vec3 Normal, float ShadowFactor)
{
	vec3 LightDirection = WorldPos0 - gPointLight.Instance[Index].Position;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	vec4 Color = CalcLightInternal(gPointLight.Instance[Index].Base, LightDirection, Normal, ShadowFactor);
	float Attenuation = gPointLight.Instance[Index].AttenConstant + 
		gPointLight.Instance[Index].AttenLinear * Distance +
		gPointLight.Instance[Index].AttenExp * Distance * Distance;
	return Color / Attenuation;
}

void main()
{
	vec3 Normal = normalize(Normal0);
	float ShadowFactor = CalcShadowFactor(WorldPos0 - gLightWorldPos);
	vec4 Light = vec4(0, 0, 0, 1);
	
	for (int i = 0; i < gDirectionalLight.Count; ++i)
	{
		Light += CalcDirectionalLight(i, Normal);
	}

	for (int i = 0; i < gPointLight.Count; ++i)
	{
		Light += CalcPointLight(i, Normal, ShadowFactor);
	}

	FragColor = texture(gMaterial.Diffuse, TexCoord0.xy) * Light;
}