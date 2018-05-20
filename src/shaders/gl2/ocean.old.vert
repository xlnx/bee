#version 300 es

uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mediump vec3 gCameraWorldPos;

#define MAX_GERSTNER_WAVE_COUNT 32
uniform float gGerstnerWave_steepness[MAX_GERSTNER_WAVE_COUNT];
uniform float gGerstnerWave_amplitude[MAX_GERSTNER_WAVE_COUNT];
uniform float gGerstnerWave_frequency[MAX_GERSTNER_WAVE_COUNT];
uniform float gGerstnerWave_speed[MAX_GERSTNER_WAVE_COUNT];
uniform vec2 gGerstnerWave_direction[MAX_GERSTNER_WAVE_COUNT];
uniform int gGerstnerWave_count;

uniform float gTime;


in vec2 Position;

// smooth out vec3 WorldPos0;
// smooth out vec3 Normal0;
out vec2 TexCoord0;
out vec3 Normal0;
out vec3 Tangent0;
out vec3 WorldPos0;

void Gerstner(vec3 vertex, out vec3 offset, out vec3 normal, out vec3 tangent)
{
	if (gGerstnerWave_count > 0)
	{
		offset = vec3(0, 0, 0);
		normal = vec3(0, 0, gGerstnerWave_count);
		tangent = vec3(0, gGerstnerWave_count, 0);

		for(int i = 0; i < gGerstnerWave_count; i++)
		{
			float dp = dot(vertex.xy, gGerstnerWave_direction[i].xy);
			float p = gGerstnerWave_frequency[i] * dp - gGerstnerWave_speed[i] * gTime;
			float Acos = gGerstnerWave_amplitude[i] * cos(p);
			float Asin = gGerstnerWave_amplitude[i] * sin(p);
			offset.x -= gGerstnerWave_steepness[i] * 
				gGerstnerWave_direction[i].x * Asin;
			offset.y -= gGerstnerWave_steepness[i] * 
				gGerstnerWave_direction[i].y * Asin;
			offset.z += Acos;

			normal.x += gGerstnerWave_frequency[i] *
				gGerstnerWave_direction[i].x * Asin;
			normal.y += gGerstnerWave_frequency[i] * 
				gGerstnerWave_direction[i].y * Asin;
			normal.z -= gGerstnerWave_frequency[i] *
				gGerstnerWave_steepness[i] * Acos;

			tangent.x -= gGerstnerWave_frequency[i] *
				gGerstnerWave_steepness[i] * 
				gGerstnerWave_direction[i].x * 
				gGerstnerWave_direction[i].y * Acos;
			tangent.y -= gGerstnerWave_frequency[i] *
				gGerstnerWave_steepness[i] * 
				gGerstnerWave_direction[i].y *
				gGerstnerWave_direction[i].y * Acos;
			tangent.z -= gGerstnerWave_frequency[i] *
				gGerstnerWave_direction[i].y * Asin;
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
	vec3 Position0 = vec3(Position, 0.0);
	Position0 += vec3(gCameraWorldPos.xy, 0.0);
	Gerstner(Position0, offset, normal, tangent);
	Position0 += offset;
	gl_Position = gWVP * vec4(Position0, 1.0);

	TexCoord0 = Position0.xy * 0.1;
	Normal0 = (gWorld * vec4(normal, 0.0)).xyz;
	Tangent0 = (gWorld * vec4(tangent, 0.0)).xyz;
	WorldPos0 = (gWorld * vec4(Position0, 1.0)).xyz;
}
