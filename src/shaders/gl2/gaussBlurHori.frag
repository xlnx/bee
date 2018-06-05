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
	for (int i = -gRange; i <= gRange; ++i)
	{
		color += texture(gImage, tex + vec2(i, 0) / iResolution.x);
	}
	FragColor = color / (float(gRange) * 2. + 1.);
}