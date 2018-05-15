#version 300 es

precision mediump float;

// camera
uniform vec3 gCameraWorldPos;
uniform mat4 gV;

// materials
uniform sampler2D gMatDiffuse;
uniform samplerCube gAmbient;
uniform float gSpecularIntensity;
uniform float gSpecularPower;



smooth in vec2 TexCoord0;
smooth in vec3 WorldPos0;
smooth in vec3 Normal0;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 NormalDepth;

void main()
{
	vec3 Normal = normalize(Normal0);
	vec3 VertexToEye = gCameraWorldPos - WorldPos0;
	vec3 R = reflect(-VertexToEye, Normal);

	const float c1 = 0.1;
	vec4 Ambient = c1 * texture(gAmbient, R);

	FragColor = Ambient + texture(gMatDiffuse, TexCoord0.xy) *
		dot(texture(gAmbient, R).xyz, vec3(1, 1, 1));
	NormalDepth = vec4(normalize(gV * vec4(Normal0, 0)).xyz, gl_FragCoord.z);
}