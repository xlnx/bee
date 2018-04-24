#version 300 es

precision mediump float;

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform sampler2D Ambient;
uniform sampler2D Emissive;
uniform sampler2D Normals;

uniform float SpecularIntensity;
uniform float SpecularPower;


in vec2 TexCoord0;

out vec4 FragColor;

void main()
{
	FragColor = texture(Diffuse, TexCoord0.xy);
}
