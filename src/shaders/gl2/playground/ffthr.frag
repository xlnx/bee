#version 300 es

precision mediump float;

uniform sampler2D gPrevH;
uniform sampler2D gPrevDx;
uniform sampler2D gPrevDy;
uniform float gN;

layout (location = 0) out vec2 H;
layout (location = 1) out vec2 Dx;
layout (location = 2) out vec2 Dy;

void main()
{
	vec2 mn = floor(gl_FragCoord.xy);
	float revx = 0.;

	for (int i = int(gN); i > 1; i /= 2)
	{	//reverse +=  bit * pow(2.0, float(i));
		revx = revx * 2. + mod(mn.x, 2.); mn.x = floor(mn.x / 2.);
	}

	vec2 tex = vec2(revx, mn.y) / gN;
	H = texture(gPrevH, tex).rg;
	Dx = texture(gPrevDx, tex).rg;
	Dy = texture(gPrevDy, tex).rg;
}