precision mediump float;

uniform vec3 gLightWorldPos;


varying vec3 WorldPos0;

void main()
{
	vec3 LightToVertex = WorldPos0 - gLightWorldPos;
	float LightToPixelDistance = length(LightToVertex);
	gl_FragColor = vec4(1, 1, 1, 1) * LightToPixelDistance;
}