#version 300 es

precision mediump float;

uniform vec3 gSunPos;


in vec3 WorldPos0;

out vec4 FragColor;

const float PI = 3.14159;

const vec4 waterColor = vec4(.24, .36, .43, 1.);

vec3 hsv2rgb(vec3 c)
{
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
	if (WorldPos0.z >= 0.0) 
	{
		vec3 sun = normalize(gSunPos);
		vec3 p = normalize(WorldPos0);

		vec3 color = vec3(0, 0, 0);

		float angle_from_origin = clamp(acos(dot(p, sun)), 0.0, PI);
		float angle_pos = asin(p.z);

		float angle_scaled = cos(sqrt(12.3 * angle_pos) - 0.8) + 0.4;

		// float atmosV = 0.25 + 0.5 * (angle_scaled);
		// float atmosS = 0.9 - angle_scaled / 18.0;
		// float atmosH = mix(0.61, 0.63, p.z);
		// color += hsv2rgb(vec3(atmosH, atmosS, atmosV));
		color += vec3(.66, .71, .76);

		const float c1 = 0.025, c2 = 0.15;
		float d = length(sun - p);
		float I = c1 / d + c2 * pow(2.0, - 5. * d);
		vec3 c = vec3(255./255.,213./255.,73./255.);

		color += c * I;

		FragColor = vec4(color, 1);
	}
	else
	{
		FragColor = waterColor;
	}
}