#version 300 es

precision mediump float;

uniform sampler2D gDepth;


in vec2 Position0;

out vec4 FragColor;

float vec42normfloat(vec4 v)
{
	const vec4 bitShift = vec4(1.0, 1.0/256.0, 1.0/(256.0*256.0), 1.0/(256.0*256.0*256.0));
	return dot(v, bitShift);
}

float getPointDepth(vec2 uv)
{
	return vec42normfloat(texture(gDepth, (uv + 1.) * .5));
}

void main()
{
	FragColor = vec4(1.) * getPointDepth(Position0);
}