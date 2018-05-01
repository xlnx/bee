#version 300 es

precision mediump float;

uniform samplerCube gAmbient;

uniform vec3 gFace;

in vec2 TexCoord0;

out vec4 FragColor;

vec3 cubelize(vec3 LightDirection, out int channel)
{
	vec3 Pos = abs(LightDirection);
	channel = Pos.x > Pos.y ? (Pos.x > Pos.z ? 0 : 2) : (Pos.y > Pos.z ? 1 : 2);
	return LightDirection / max(Pos.x, max(Pos.y, Pos.z));
}

vec4 SampleCube(vec3 face, vec2 Pos)
{
	int channel;
	vec3 norm = cubelize(face, channel);
	if (channel == 0)
		return texture(gAmbient, vec3(norm.x, Pos.x, Pos.y));
	else if (channel == 1)
		return texture(gAmbient, vec3(Pos.x, norm.y, Pos.y));
	else 
		return texture(gAmbient, vec3(Pos.x, Pos.y, norm.z));
}

void main()
{
	FragColor = SampleCube(gFace, TexCoord0 * 2.0 - 1.0);
}