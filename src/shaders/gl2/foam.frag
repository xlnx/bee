#version 300 es

precision mediump float; 

uniform sampler2D gOutline;
uniform sampler2D gFoam;

in vec2 TexCoord0;

out vec4 FragColor;

void main()
{
	FragColor = vec4(vec3(1.), 
		texture(gOutline, TexCoord0).a * 
			texture(gFoam, vec2(TexCoord0.x, TexCoord0.y * 10.)).a);
		// vec4(1.);
}