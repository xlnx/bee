#version 300 es

precision mediump float;

uniform sampler2D gPrevStep;
uniform float gN;

out vec2 FragColor;

void main()
{
	vec2 mn = floor(gl_FragCoord.xy);
	FragColor = (1. - 2. * mod(mn.x + mn.y, 2.)) * 
		texture(gPrevStep, mn / gN).xx * 2e-5;
}

// out vec2 FragColor;

// void main()
// {
// 	vec2 mn = floor(gl_FragCoord.xy);
// 	FragColor = //(mod(mn.x + mn.y, 2.) == 1. ? -1.: 1.) * 
// 		texture(gPrevStep, mn / gN).xy;// * 8e-5;
// }