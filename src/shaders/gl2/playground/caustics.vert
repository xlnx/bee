#version 300 es

uniform sampler2D gNormalHeight;
uniform vec3 gLight;
uniform mat4 gWorld;

in vec2 Position;

out vec3 OldPos;
out vec3 NewPos;

void main()
{
	vec2 uv = Position;
	vec4 nh = texture(gNormalHeight, uv * .5 + .5);
	vec3 p = (gWorld * vec4(uv, nh.w * 2. - 1., 1.)).xyz;
	vec3 n = nh.xyz;
	vec3 n0 = vec3(0., 0., 1.);
	vec3 ray = refract(gLight, n, 1.0 / 1.33);
	vec3 ray0 = refract(gLight, n0, 1.0 / 1.33);

	OldPos = p + (-p.z - 1.) * ray0 / ray0.z;
	NewPos = p + (-p.z - 1.) * ray / ray.z; 

	gl_Position = vec4(NewPos.xy, 0, 1); //vec4(uv, 0, 1);
}