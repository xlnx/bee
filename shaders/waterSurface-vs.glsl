#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 2) in vec3 Normal;
layout (location = 5) in vec2 TexCoord;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;
out vec3 Offset0;
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

void GerstnerLevelOne(vec3 vertex, out vec3 offset, out vec3 normal)
{
	offset = vec3(0, 0, 0);
	normal = vec3(0, 0, 1);

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
	}
}

void main()
{
	vec3 offset;
	vec3 normal;
	vec3 Position0 = Position;
	GerstnerLevelOne(Position0, offset, normal);
	Position0 += offset;
	gl_Position = gWVP * vec4(Position0, 1.0);
	// TexCoord0 = TexCoord;
	if (normal.z < 0)
		normal.z = -normal.z;
	Normal0 = (gWorld * vec4(normal, 0.0)).xyz;
	WorldPos0 = (gWorld * vec4(Position0, 1.0)).xyz;
	// Color0 = vec4(clamp(Position0, 0.0, 1.0), 1.0);
	// gl_Position = gWVP * vec4(Position, 1.0);
	Offset0 = offset;
}