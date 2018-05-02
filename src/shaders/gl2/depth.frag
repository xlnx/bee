#version 300 es

precision mediump float;

out vec4 FragColor;

vec4 normfloat2vec4(float x)
{
	const vec4 bitShift = vec4(1.0, 256.0, 256.0 * 256.0, 256.0 * 256.0 * 256.0);
	const vec4 bitMask = vec4(1.0/256.0, 1.0/256.0, 1.0/256.0, 0.0);
	vec4 rgba = fract(x * bitShift);
	rgba -= rgba.gbaa * bitMask;
	return rgba;
}

float encodeDepth(float d) 
{
	return exp(- d * d - 1.);
	return d;
}

void main()
{
	FragColor = normfloat2vec4(encodeDepth(gl_FragCoord.z));
}