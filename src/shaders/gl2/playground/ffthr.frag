#version 300 es

precision mediump float;

uniform sampler2D gPrevStep;
uniform float gN;

out vec2 FragColor;

void main()
{
	vec2 mn = floor(gl_FragCoord.xy);
	float revx = 0.;

	for (int i = int(gN); i > 1; i /= 2)
	{	//reverse +=  bit * pow(2.0, float(i));
		revx = revx * 2. + mod(mn.x, 2.); mn.x = floor(mn.x / 2.);
	}

	FragColor = texture(gPrevStep, vec2(revx, mn.y) / gN).rg;
}