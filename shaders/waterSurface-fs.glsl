#version 330 core

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 Tangent0;
in vec3 WorldPos0;
// in vec3 Offset0;
// in vec4 Color0;

out vec4 FragColor;

struct Material
{
	sampler2D Diffuse;
	sampler2D Specular;
	sampler2D Ambient;
	sampler2D Emissive;
	sampler2D Normals;

	float SpecularIntensity;
	float SpecularPower;
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
			SpecularFactor = pow(SpecularFactor, gMaterial.SpecularPower);
			SpecularColor = vec4(SpecularFactor * gMaterial.SpecularIntensity * Light.Color, 1.0f);
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

vec3 CalcBumpedNormal()
{
	vec3 Normal = normalize(Normal0);
	vec3 Tangent = normalize(Tangent0);
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	vec3 Bitangent = cross(Tangent, Normal);
	vec3 BumpMapNormal = texture(gMaterial.Normals, TexCoord0).xyz;
	BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
	vec3 NewNormal;
	mat3 TBN = mat3(Tangent, Bitangent, Normal);
	NewNormal = TBN * BumpMapNormal;
	NewNormal = normalize(NewNormal);
	return NewNormal;
} 

void main()
{
	vec3 Normal = CalcBumpedNormal();
	vec4 Light = CalcDirectionalLight(Normal);

	for (int i = 0; i < gPointLightCount; ++i)
	{
		Light += CalcPointLight(i, Normal);
	}

	// FragColor = vec4(Normal, 1);
	FragColor = vec4(0.2, 0.32, 0.45, 1)
	/*texture2D(gMaterial.Diffuse, TexCoord0.xy)*/ * Light;
}