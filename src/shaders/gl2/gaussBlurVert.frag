#version 300 es

precision mediump float;

uniform sampler2D gImage;
uniform vec2 iResolution;
uniform int gRange;


in vec2 Position0;

out vec4 FragColor;

void main()
{
	vec2 tex = (Position0 + 1.) * .5;
	vec4 color = vec4(0.);
	for (int j = -gRange; j <= gRange; ++j)
	{
		color += texture(gImage, tex + vec2(0, j) / iResolution.y);
	}
	FragColor = color / (float(gRange) * 2. + 1.);
}