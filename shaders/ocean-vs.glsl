#version 330 core

layout (location = 0) in vec2 Position;
layout (location = 2) in vec3 Normal;
// layout (location = 5) in vec2 TexCoord;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 Tangent0;
out vec3 WorldPos0;
// out vec3 Offset0;
// out vec4 Color0;

#define MAX_GERSTNER_WAVE_COUNT 128

struct GerstnerWave {
	float Steepness;
	float Amplitude;
	float Frequency;
	float Speed;
	vec2 Direction;
};

uniform GerstnerWave gGerstnerWave[MAX_GERSTNER_WAVE_COUNT];
uniform int gGerstnerWaveCount;
uniform float gTime;

uniform mat4 gWVP;
uniform mat4 gWorld;
uniform vec3 gCameraWorldPos;

void Gerstner(vec3 vertex, out vec3 offset, out vec3 normal, out vec3 tangent)
{
	if (gGerstnerWaveCount > 0)
	{
		offset = vec3(0, 0, 0);
		normal = vec3(0, 0, gGerstnerWaveCount);
		tangent = vec3(0, gGerstnerWaveCount, 0);

		for(int i = 0; i < gGerstnerWaveCount; i++)
		{
			float dp = dot(vertex.xy, gGerstnerWave[i].Direction.xy);
			float p = gGerstnerWave[i].Frequency * dp - gGerstnerWave[i].Speed * gTime;
			float Acos = gGerstnerWave[i].Amplitude * cos(p);
			float Asin = gGerstnerWave[i].Amplitude * sin(p);
			offset.x -= gGerstnerWave[i].Steepness * 
				gGerstnerWave[i].Direction.x * Asin;
			offset.y -= gGerstnerWave[i].Steepness * 
				gGerstnerWave[i].Direction.y * Asin;
			offset.z += Acos;

			normal.x += gGerstnerWave[i].Frequency *
				gGerstnerWave[i].Direction.x * Asin;
			normal.y += gGerstnerWave[i].Frequency * 
				gGerstnerWave[i].Direction.y * Asin;
			normal.z -= gGerstnerWave[i].Frequency *
				gGerstnerWave[i].Steepness * Acos;

			tangent.x -= gGerstnerWave[i].Frequency *
				gGerstnerWave[i].Steepness * 
				gGerstnerWave[i].Direction.x * 
				gGerstnerWave[i].Direction.y * Acos;
			tangent.y -= gGerstnerWave[i].Frequency *
				gGerstnerWave[i].Steepness * 
				gGerstnerWave[i].Direction.y *
				gGerstnerWave[i].Direction.y * Acos;
			tangent.z -= gGerstnerWave[i].Frequency *
				gGerstnerWave[i].Direction.y * Asin;
		}
		normal = normalize(normal);
		tangent = normalize(tangent);
	}
	else
	{
		offset = vec3(0, 0, 0);
		normal = vec3(0, 0, 1);
		tangent = vec3(0, 1, 0);
	}
}

void main()
{
	vec3 offset;
	vec3 normal;
	vec3 tangent;
	// vec3 Position0 = vec3(Position * (abs(gCameraWorldPos.z) + 1), 0);
	vec3 Position0 = vec3(Position, 0.0);
	Position0 += vec3(gCameraWorldPos.xy, 0.0);
	Gerstner(Position0, offset, normal, tangent);
	Position0 += offset;
	gl_Position = gWVP * vec4(Position0, 1.0);

	TexCoord0 = Position0.xy * 0.1;
	Normal0 = (gWorld * vec4(normal, 0.0)).xyz;
	Tangent0 = (gWorld * vec4(tangent, 0.0)).xyz;
	
	WorldPos0 = (gWorld * vec4(Position0, 1.0)).xyz;
	// Offset0 = offset;
}