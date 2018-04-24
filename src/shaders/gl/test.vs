uniform mat4 gWVP;


attribute vec3 Position;
attribute vec3 v1;
attribute vec3 v2;
attribute vec3 v3;
attribute vec3 v4;
attribute vec2 TexCoord;

varying vec2 TexCoord0;
varying vec3 V1;
varying vec3 V2;
varying vec3 V3;
varying vec3 V4;

void main()
{
	gl_Position = gWVP * vec4(Position, 1);
	TexCoord0 = TexCoord;
	V1 = v1; V2 = v2; V3 = v3; V4 = v4;
}
