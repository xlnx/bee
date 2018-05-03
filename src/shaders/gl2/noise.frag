#version 300 es

precision mediump float;

in vec2 Position0;

out vec3 FragColor;

float rand(vec3 n)
{
	return fract(sin(dot(n, vec3(12.9898, 4.1414, 5.87924))) * 43758.5453);
}

float Noise3D(vec3 p)
{
	vec2 e = vec2(0.0, 1.0);
	vec3 i = floor(p);
	vec3 f = fract(p);
	
	float x0 = mix(rand(i + e.xxx), rand(i + e.yxx), f.x);
	float x1 = mix(rand(i + e.xyx), rand(i + e.yyx), f.x);
	float x2 = mix(rand(i + e.xxy), rand(i + e.yxy), f.x);
	float x3 = mix(rand(i + e.xyy), rand(i + e.yyy), f.x);
	
	float y0 = mix(x0, x1, f.y);
	float y1 = mix(x2, x3, f.y);
	
	float val = mix(y0, y1, f.z);
	
	val = val * val * (3.0 - 2.0 * val);
	return val;
}

void main()
{
	FragColor = vec3(1.) * Noise3D(vec3(Position0, 0));
}