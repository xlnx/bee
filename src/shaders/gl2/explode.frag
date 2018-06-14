#version 300 es

#define PI 3.141592654

#define RAYMARCH_ITER_STEP .004
#define RAYMARCH_MAX_ITER 20

precision mediump float;

uniform mat4 gV;
uniform mat4 gIP;
uniform vec3 gLightDir;
uniform vec3 gCameraWorldPos;
uniform float gLifetime;
uniform float gTime;

in vec4 WorldPos_next;
in float InstanceId_next;
in float GroupId_next;
in float Lifetime_next;

out vec4 FragColor;

const vec3 fire = vec3(.5, .15, .05);
const vec3 water = vec3(1.);

float hash( float n )
{
	return fract(cos(n)*41415.92653);
}

// 3d noise function
float noise( in vec3 x )
{
	vec3 p  = floor(x);
	vec3 f  = smoothstep(0.0, 1.0, fract(x));
	float n = p.x + p.y*57.0 + 113.0*p.z;

	return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
		mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
		mix(mix( hash(n+113.0), hash(n+114.0),f.x),
		mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}


mat3 m = mat3( 0.00,  1.60,  1.20, -1.60,  0.72, -0.96, -1.20, -0.96,  1.28 );

// Fractional Brownian motion
float fbm( vec3 p )
{
	float f = 0.5000*noise( p ); p = m*p*1.1;
	f += 0.2500*noise( p ); p = m*p*1.2;
	f += 0.1666*noise( p ); p = m*p;
	f += 0.0834*noise( p );
	return f;
}

float raymarchDepth(vec3 p)
{
	vec3 ro = gCameraWorldPos;
	vec3 rd = normalize(p - ro);

	float t = 0.;
	vec4 z = vec4(0.);
	for (int i = 0; i < RAYMARCH_MAX_ITER; ++i)
	{
		if (z.a > 0.99) break;
		vec3 pos = ro + t * rd;
		float den = fbm(pos + vec3(0, 0, 0.));
		vec4 col = vec4(mix(vec3(1.), vec3(0.), den), den);
		col.rgb *= col.a;
		z += col * (1.0 - z.a);
		t += max(RAYMARCH_ITER_STEP, (2. - den) * t);
	}
	return z.r * .5;
}

void main()
{
	// FragColor = vec4(1., 0., 0., 1.);
	vec2 pp = gl_FragCoord.xy / vec2(1200, 900) * 2. - 1.;
	vec3 i = normalize(- (gIP * vec4(pp, 1, 1)).xyz);
	vec2 p0 = gl_PointCoord.xy * 2. - 1.;
	float l = length(p0);
	float t = Lifetime_next / gLifetime;
	vec3 p = vec3(p0, -1. + gTime * .3);
	float den = t * -log(t) * 3.;

	den *= (1. - fbm(p + sin(InstanceId_next))) * (1. - l * l);
	den = WorldPos_next.z > 0. ? den : 0.;
	vec3 n = normalize(-vec3(dFdx(den), dFdy(den), -.01));
	vec3 vi = normalize(- (gV * vec4(gLightDir, 0)).xyz);
	vec3 h = (vi + i) * .5;
	float difu = max(dot(h, n), 0.) * .5;
	float amb = .3;
	FragColor = //vec4(vi * .5 + .5, 1);
	InstanceId_next == 9. || InstanceId_next < 3. ? 
	vec4((difu + amb) * fire, den)
	:
	vec4((difu + amb) * water, den);
	// vec4((difu + amb) * water, den);
	// vec4(vec3(6.), den);
	
	// vec4(raymarchDepth(p));
	// vec4(vec2(raymarchDepth(p)), 0, 1);// * t * -log(t);
	// vec4(gl_PointCoord.xy, 0, 1);
}