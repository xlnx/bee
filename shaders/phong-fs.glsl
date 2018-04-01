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
} gMaterial;

uniform vec3 gCameraWorldPos;
uniform vec3 gLightWorldPos;
uniform samplerCube gShadowMap;
// uniform vec2 gShadowMapScale;

// float ShadowMapOffsetLookup(vec3 plain, vec3 offset)
// {
// 	return texture(gShadowMap, plain + offset).r;
// }

const float shadowMapScale = 1.0 / 512.0;

float ShadowMapOffsetLookup(vec3 plain, vec3 offset)
{
	return texture(gShadowMap, plain + offset * shadowMapScale).r;
}

vec3 Plainlize(vec3 LightDirection, out int channel)
{
	vec3 Pos = abs(LightDirection);
	float maxD;
	if (Pos.x > Pos.y)
	{
		if (Pos.x > Pos.z) 
		{
			channel = 0; maxD = Pos.x;
		}
		else
		{
			channel = 2; maxD = Pos.z;
		}
	}
	else
	{
		if (Pos.y > Pos.z)
		{
			channel = 1; maxD = Pos.y;
		}
		else
		{
			channel = 2; maxD = Pos.z;
		}
	}
	return LightDirection / maxD;
}

float ForcePCFShadowDistance4x4(vec3 LightDirection)
{
	int channel;
	vec3 PosPlain = Plainlize(LightDirection, channel);
	float SampledDistance = 0;
	float x, y;
	switch (channel)
	{
	case 0:
		for (y = -1.5; y <= 1.5; y += 1.0)
		{
			for (x = -1.5; x <= 1.5; x += 1.0)
			{
				SampledDistance += ShadowMapOffsetLookup(PosPlain, vec3(0, x, y));
				// vec2 coord = gShadowMapScale * vec2(x, y);
				// SampledDistance += ShadowMapOffsetLookup(PosPlain, vec3(0, coord.x, coord.y));
			}
		} break;
	case 1:
		for (y = -1.5; y <= 1.5; y += 1.0)
		{
			for (x = -1.5; x <= 1.5; x += 1.0)
			{
				SampledDistance += ShadowMapOffsetLookup(PosPlain, vec3(x, 0, y));
			}
		} break;
	case 2:
		for (y = -1.5; y <= 1.5; y += 1.0)
		{
			for (x = -1.5; x <= 1.5; x += 1.0)
			{
				SampledDistance += ShadowMapOffsetLookup(PosPlain, vec3(x, y, 0));
			}
		} break;
	}
	return SampledDistance * 0.0625;// / 16.0;
}

float PCFShadowDistance4(vec3 LightDirection)
{
	//int channel;
	//vec3 PosPlain = Plainlize(LightDirection, channel);
	//vec2 offset = (LightDirection.xy * 0.5) > 0.25;
	//offset.y += offset.x;
	//if (offset.y > 1.1)
	//return (
	//	ShadowMapOffsetLookup(PosPlain, vec3())
	//) * 0.25;
	return 1;
}

float SimpleShadowDistance(vec3 LightDirection)
{
	return texture(gShadowMap, LightDirection).r;
}

float CalcShadowFactor(vec3 LightDirection)
{
	float SampledDistance = 
		// SimpleShadowDistance(LightDirection);
		ForcePCFShadowDistance4x4(LightDirection);
	float Distance = length(LightDirection);
	float diff = Distance - SampledDistance;
	if (diff < 1e-1)
	{
		return 1.0;
	}
	else
	{
		return 0.4;
	}
}

void CalcLightInternal(LightBase Light, vec3 LightDirection, vec3 Normal, 
	float ShadowFactor, out vec3 Light0, out vec3 LightSpec0)
{
	vec3 AmbientColor = vec3(Light.Color * Light.AmbientIntensity);
	float DiffuseFactor = dot(Normal, -LightDirection);
	
	vec3 DiffuseColor = vec3(0, 0, 0);
	vec3 SpecularColor = vec3(0, 0, 0);

	if (DiffuseFactor > 0)
	{
		DiffuseColor = vec3(Light.Color * Light.DiffuseIntensity * DiffuseFactor);

		vec3 VertexToEye = normalize(gCameraWorldPos - WorldPos0);
		vec3 LightReflect = normalize(reflect(LightDirection, Normal));
		float SpecularFactor = dot(VertexToEye, LightReflect);
		if (SpecularFactor > 0)
		{
			SpecularFactor = pow(SpecularFactor, gMaterial.SpecularPower);
			SpecularColor = SpecularFactor * gMaterial.SpecularIntensity * Light.Color;
		}
	}
	// return AmbientColor + ShadowFactor * (DiffuseColor + SpecularColor);
	// LightSpec0 = ShadowFactor * SpecularColor;
	Light0 = AmbientColor + ShadowFactor * DiffuseColor;
	LightSpec0 = step(1.0, ShadowFactor) * SpecularColor;
}

void CalcDirectionalLight(int Index, vec3 Normal, out vec3 Light, out vec3 LightSpec)
{
	vec3 Light0, LightSpec0;
	CalcLightInternal(gDirectionalLight.Instance[Index].Base, 
		gDirectionalLight.Instance[Index].Direction, Normal, 1.0, Light0, LightSpec0);
	Light += Light0; LightSpec += LightSpec0;
}

void CalcPointLight(int Index, vec3 Normal, float ShadowFactor, out vec3 Light, out vec3 LightSpec)
{
	vec3 LightDirection = WorldPos0 - gPointLight.Instance[Index].Position;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	vec3 Light0, LightSpec0;
	CalcLightInternal(gPointLight.Instance[Index].Base, LightDirection, Normal, ShadowFactor, Light0, LightSpec0);
	float Attenuation = gPointLight.Instance[Index].AttenConstant + 
		gPointLight.Instance[Index].AttenLinear * Distance +
		gPointLight.Instance[Index].AttenExp * Distance * Distance;
	Light += Light0 / Attenuation; LightSpec += LightSpec0;
}

void main()
{
	vec3 Normal = normalize(Normal0);
	float ShadowFactor = CalcShadowFactor(WorldPos0 - gLightWorldPos);
	vec3 Light = vec3(0, 0, 0), LightSpec = vec3(0, 0, 0);
	
	for (int i = 0; i < gDirectionalLight.Count; ++i)
	{
		CalcDirectionalLight(i, Normal, Light, LightSpec);
	}

	for (int i = 0; i < gPointLight.Count; ++i)
	{
		CalcPointLight(i, Normal, ShadowFactor, Light, LightSpec);
	}

	FragColor = texture(gMaterial.Diffuse, TexCoord0.xy) * vec4(Light, 1.0f) + vec4(LightSpec, 1.0);
	// FragColor = vec4(1);
}
