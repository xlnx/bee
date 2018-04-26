uniform mat4 gWVP;
uniform mat4 gWorld;


varying vec3 Position;
varying vec3 Normal;
varying vec2 TexCoord;

varying vec3 WorldPos0;

void main()
{
	vec4 Pos4 = vec4(Position, 1.0);
	gl_Position = gWVP * Pos4;
	WorldPos0 = (gWorld * Pos4).xyz;
}