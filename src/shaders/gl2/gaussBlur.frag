#version 300 es

precision mediump float;

uniform sampler2D gImage;
uniform vec2 iResolution;
uniform float gStep;
uniform int gRange;


in vec2 Position0;

out vec4 FragColor;

void main()
{
	vec2 uv = Position0;
	vec4 color = vec4(0.);
	vec2 offset = vec2(gStep / iResolution.x, gStep / iResolution.y);
	for (int i = -gRange; i <= gRange; ++i)
	{
		for (int j = -gRange; j <= gRange; ++j)
		{
			color += texture(gImage, (uv + 1.) * .5 + offset * vec2(i, j));
		}
	}
	FragColor = color / (float(gRange) * 2. + 1.) / (float(gRange) * 2. + 1.);
}