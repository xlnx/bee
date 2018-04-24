precision mediump float;

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform sampler2D Ambient;
uniform sampler2D Emissive;
uniform sampler2D Normals;

uniform float SpecularIntensity;
uniform float SpecularPower;


varying vec2 TexCoord0;

void main()
{
	gl_FragColor = texture2D(Diffuse, TexCoord0.xy);
}
