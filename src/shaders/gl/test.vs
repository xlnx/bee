uniform mat4 gWVP;
uniform mat4 gWorld;


attribute vec3 Position;
attribute vec3 Normal;
attribute vec2 TexCoord;

varying vec2 TexCoord0;
varying vec3 WorldPos0;
varying vec3 Normal0;

void main()
{
	gl_Position = gWVP * vec4(Position, 1);
	TexCoord0 = TexCoord;
	Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;
	WorldPos0 = (gWorld * vec4(Position, 1.0)).xyz;
}
