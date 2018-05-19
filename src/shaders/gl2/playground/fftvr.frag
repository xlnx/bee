#version 300 es

precision mediump float;

uniform sampler2D gPrevStep;
uniform float gN;

out vec2 FragColor;

void main()
{
	vec2 mn = floor(gl_FragCoord.xy);
	float revy = 0.;

	for (int i = int(gN); i > 1; i /= 2)
	{	//reverse +=  bit * pow(2.0, float(i));
		revy = revy * 2. + mod(mn.y, 2.); mn.y = floor(mn.y / 2.);
	}

	FragColor = texture(gPrevStep, vec2(mn.x, revy) / gN).rg;
}