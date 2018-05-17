#version 300 es

precision mediump float;

uniform sampler2D gCaustics;
uniform sampler2D gTile;


in vec3 Normal0;
in vec2 Position0;
in vec2 TexCoord0;
in vec3 WorldPos0;

out vec4 FragColor;

void main()
{
	if (WorldPos0.z >= 0.) {
		FragColor = texture(gTile, TexCoord0);
			// vec4(1.) * texture(gCaustics, Position0 * .5 + .5).r;
	} else {
		FragColor = (texture(gTile, TexCoord0) + vec4(.2, .6, .7, 1.)) *
			(texture(gCaustics, Position0 * .5 + .5).r + .2);
	}
}