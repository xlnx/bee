#version 300 es
// #extension GL_OES_standard_derivatives : enable

precision mediump float;

in vec3 OldPos;
in vec3 NewPos;

out float FragColor;

void main()
{
	float oldArea = length(dFdx(OldPos)) * length(dFdy(OldPos));
	float newArea = length(dFdx(NewPos)) * length(dFdy(NewPos));
	FragColor = oldArea / newArea * .2;
	//(oldArea / newArea - 1.) * .2;
}