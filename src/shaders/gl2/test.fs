#version 300 es

precision mediump float;

// camera
uniform vec3 gCameraWorldPos;

// materials
uniform sampler2D gDiffuse;
uniform samplerCube gAmbient;
uniform float gSpecularIntensity;
uniform float gSpecularPower;



smooth in vec2 TexCoord0;
smooth in vec3 WorldPos0;
smooth in vec3 Normal0;

out vec4 FragColor;

void main()
{
	vec3 Normal = normalize(Normal0);
	vec3 VertexToEye = gCameraWorldPos - WorldPos0;
	vec3 R = reflect(-VertexToEye, Normal);

	const float c1 = 0.1;
	vec4 Ambient = c1 * texture(gAmbient, R);

	FragColor = Ambient + texture(gDiffuse, TexCoord0.xy) *
		dot(texture(gAmbient, R).xyz, vec3(1, 1, 1));
}