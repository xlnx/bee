#version 300 es

precision mediump float;

uniform mat4 gV;


in vec3 Normal0;

out vec4 FragColor;

void main()
{
	FragColor = vec4(normalize(gV * vec4(Normal0, 0)).xyz, gl_FragCoord.z);
}