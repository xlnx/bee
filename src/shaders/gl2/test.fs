#version 300 es

in lowp vec2 TexCoord0;

out lowp vec4 FragColor;

uniform struct
{
	sampler2D Diffuse;
	sampler2D Specular;
	sampler2D Ambient;
	sampler2D Emissive;
	sampler2D Normals;

	lowp float SpecularIntensity;
	lowp float SpecularPower;
} gMaterial;

void main()
{
	FragColor = texture(gMaterial.Diffuse, TexCoord0.xy);
}
