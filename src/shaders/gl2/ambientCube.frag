#version 300 es

precision mediump float;

uniform vec3 gSunPos;


in vec3 WorldPos0;

out vec4 FragColor;

const float PI = 3.14159;

const vec4 shallowWaterColor = vec4(.24, .36, .43, 1.);
const vec4 deepWaterColor = vec4(.11, .17, .20, 1.);

const vec4 shallowWaterColorNight = vec4(.23, .28, .33, 1.);
const vec4 deepWaterColorNight = vec4(.08, .14, .16, 1.);

vec3 hsv2rgb(vec3 c)
{
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
	vec3 sun = normalize(gSunPos);
	if (WorldPos0.z > 0.0) 
	{
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
		float ang = dot(sun, vec3(0, 0, 1));
		vec4 dayColor = mix(shallowWaterColor, deepWaterColor, clamp(-WorldPos0.z * 1.2, 0., 1.));
		vec4 nightColor = mix(shallowWaterColorNight, deepWaterColorNight, clamp(-WorldPos0.z * 1.2, 0., 1.));
		FragColor = mix(nightColor, dayColor, ang);
	}
}

// #version 300 es

// precision mediump float;

// uniform vec3 gSunPos;



// in vec3 WorldPos0;

// out vec4 FragColor;

// const mat3 mRGB2XYZ = mat3(0.5141364, 0.3238786 , 0.16036376,
// 				           0.265068 , 0.67023428, 0.06409157,
// 					       0.0241188, 0.1228178 , 0.84442666);

// const mat3 mXYZ2RGB = mat3( 2.5651, -1.1665, -0.3986,
// 				           -1.0217,  1.9777,  0.0439,
// 					        0.0753, -0.2543,  1.1892);

// vec3 rgb_to_yxy(vec3 cRGB)
// {
// 	vec3 cYxy;
//     vec3 cXYZ = mRGB2XYZ * cRGB;
//     cYxy.r = cXYZ.g;
//     float temp = dot(vec3(1.0, 1.0, 1.0), cXYZ.rgb); 
//     cYxy.gb = cXYZ.rg / temp;
//     return cYxy;
// }

// vec3 yxy_to_rgb(vec3 cYxy)
// {
//     vec3 cXYZ = vec3(cYxy.r * cYxy.g / cYxy.b,
//                      cYxy.r,
//                      cYxy.r * (1.0 - cYxy.g - cYxy.b) / cYxy.b);
//     return mXYZ2RGB * cXYZ;
// }

// const vec3 vGammaPowerInverse = vec3(2.2);
// const vec3 vGammaPower = vec3(0.454545);

// vec3 linear_to_gamma(vec3 c)
// {
// 	return pow(clamp(c, 0.0, 1.0), vGammaPower);
// }

// vec3 gamma_to_linear(vec3 c)
// {
// 	return pow(clamp(c, 0.0, 1.0), vGammaPowerInverse);
// }


// const float PI = 3.14159265358979323846;
// const float PI_2 = 1.57079632679489661923;
// const float PI_4 = 0.785398163397448309616;
// const float earth_rs = 6360.0e3;
// const float earth_ra = 6420.0e3;
// const vec3 earth_beta_r = vec3(5.5e-6, 13.0e-6, 22.1e-6);
// const vec3 earth_beta_m = vec3(18.0e-6);
// const float earth_sh_r = 7994.0;
// const float earth_sh_m = 1100.0;
// const float sun_intensity = 15.0;
// const float sun_mc = 0.76;
// const float sun_ad = 0.009250245;
// const vec3 sun_color = vec3(1.,1.,1.);
// float sun_alt;


// bool intersect_with_atmosphere(in vec3 ro, in vec3 rd, out float tr)
// {
// 	float c = length(ro); // distance from center of the planet :)
// 	vec3 up_dir = ro / c;
// 	float beta = PI - acos(dot(rd, up_dir)); 
// 	float sb = sin(beta);
// 	float b = earth_ra;
// 	float bt = earth_rs - 10.0;
	
// 	tr = sqrt((b * b) - (c * c) * (sb * sb)) + c * cos(beta); // sinus law
	
// 	if (sqrt((bt * bt) - (c * c) * (sb * sb)) + c * cos(beta) > 0.0)
// 		return false;
	
// 	return true;
// }

// const int SKYLIGHT_NB_VIEWDIR_SAMPLES = 8;
// const int SKYLIGHT_NB_SUNDIR_SAMPLES = 4;

// float compute_sun_visibility(float alt)
// {
// 	float vap = 0.0;
// 	float h, a;
// 	float vvp = clamp((0.5 + alt / sun_ad), 0.0, 1.0); // vertically visible percentage
// 	if (vvp == 0.0)
// 		return 0.0;
// 	else if (vvp == 1.0)
// 		return 1.0;
		
// 	bool is_sup;
	
// 	if (vvp > 0.5)
// 	{
// 		is_sup = true;
// 		h = (vvp - 0.5) * 2.0;
// 	}
// 	else
// 	{
// 		is_sup = false;
// 		h = (0.5 - vvp) * 2.0;
// 	}
	
// 	float alpha = acos(h) * 2.0;
// 	a = (alpha - sin(alpha)) / (2.0 * PI);
	
// 	if (is_sup)
// 		vap = 1.0 - a;
// 	else
// 		vap = a;

// 	return vap;
// }

// // pred : rd is normalized
// vec3 compute_sky_light(in vec3 ro, in vec3 rd, in vec3 sun)
// {
//     float t1;
	
//     if (!intersect_with_atmosphere(ro, rd, t1) || t1 < 0.0)
// 		return vec3(0.0);
    
//     float sl = t1 / float(SKYLIGHT_NB_VIEWDIR_SAMPLES); // seg length
//     float t = 0.0;

// 	vec3 sun_dir = sun;
//     // vec3 sun_dir = vec3(0.,0.4,-0.9);
// 	float mu = dot(rd, sun_dir);
    
// 	float mu2 = mu * mu;
// 	float mc2 = sun_mc * sun_mc;
	
// 	// rayleigh stuff
// 	vec3 sumr = vec3(0.0);
//     float odr = 0.0; // optical depth
// 	float phase_r = (3.0 / (16.0 * PI)) * (1.0 + mu2);
	
// 	// mie stuff
// 	vec3 summ = vec3(0.0);
// 	float odm = 0.0; // optical depth
// 	float phase_m = ((3.0 / (8.0 * PI)) * ((1.0 - mc2) * (1.0 + mu2))) /
// 		            ((2.0 + mc2) * pow(1.0 + mc2 - 2.0 * sun_mc * mu, 1.5));
    
//     for (int i = 0; i < SKYLIGHT_NB_VIEWDIR_SAMPLES; ++i)
// 	{
// 		vec3 sp = ro + rd * (t + 0.5 * sl);
//         float h = length(sp) - earth_rs;
//         float hr = exp(-h / earth_sh_r) * sl;
// 		odr += hr;
//         float hm = exp(-h / earth_sh_m) * sl;
//         odm += hm;
// 		float tm;
// 		float sp_alt = PI_2 - asin(earth_rs / length(sp));
// 		sp_alt += acos(normalize(sp).y) + sun_alt;
// 		float coef = compute_sun_visibility(sp_alt);
// 		if (intersect_with_atmosphere(sp, sun_dir, tm) || coef > 0.0)
// 		{
// 			float sll = tm / float(SKYLIGHT_NB_SUNDIR_SAMPLES);
// 			float odlr = 0.0;
//             float odlm = 0.0;
// 			for (int j = 0; j < SKYLIGHT_NB_SUNDIR_SAMPLES; ++j)
// 			{
// 				vec3 spl = sp + sun_dir * ((float(j) + 0.5) * sll);
// 				float spl_alt = PI_2 - asin(earth_rs / length(spl));
// 				spl_alt += acos(normalize(spl).y) + sun_alt;
// 				float coefl = compute_sun_visibility(spl_alt);
// 				float hl = length(spl) - earth_rs;
// 				odlr += exp(-hl / earth_sh_r) * sll * (1.0 - log(coefl + 0.000001));
// 				odlm += exp(-hl / earth_sh_m) * sll * (1.0 - log(coefl + 0.000001));
				
// 			}
//             vec3 tau_m = earth_beta_m * 1.05 * (odm + odlm);
// 			vec3 tau_r = earth_beta_r * (odr + odlr);
//             vec3 tau = tau_m + tau_r;
// 			vec3 attenuation = exp(-tau);
// 			sumr += hr * attenuation * coef;
// 			summ += hm * attenuation * coef;
// 		}
//         t += sl;
//     }
//     float direct_coef = 1.0;
// 	if (acos(mu) < sun_ad * 0.6) // makes it a bit bigger
//     	direct_coef = 3.0 + sin(mu / (sun_ad * 0.5)) * 3.0;
//     return 0.8 * sun_intensity * direct_coef * (sumr * phase_r * earth_beta_r + summ * phase_m * earth_beta_m);
// }

// //===============================================================================================
// // MAIN
// //===============================================================================================

// void main()
// {
// 	if (WorldPos0.z >= 0.0) 
// 	{
// 		// vec3(0.0, .0, -2.0)
// 		vec3 sun = normalize(vec3(-gSunPos.y, gSunPos.z, -gSunPos.x));
// 		sun_alt = asin(sun.y);
		
// 		// vec2 q = gl_FragCoord.xy / iResolution.xy;
// 		// vec2 p = -1.0 + 2.0*q;
// 		// p.x *= iResolution.x / iResolution.y;
// 		// float view_coef = 0.8;

// 		// if (sun.y < 0.0)
// 		// {
// 		// 	FragColor = vec4(sun * .5 + .5, 1.);
// 		// 	// vec4(1.);
// 		// 	return;
// 		// }

// 		// vec3 c_position = vec3(0.0, 1.0, 0.0);
// 		// vec3 c_lookat   = vec3(-0.04, 1.0, -1.0);
// 		// vec3 c_updir    = vec3(0.0, 1.0, 0.0);
		
// 		// vec3 view_dir = normalize(c_lookat - c_position);
		
// 		// vec3 uu = normalize(cross(view_dir, c_updir));
// 		// vec3 vv = normalize(cross(uu, view_dir));
// 		// vec3 rd = normalize(p.x * uu + p.y * vv + view_coef * view_dir);

// 		vec3 rd = normalize(vec3(-WorldPos0.y, WorldPos0.z, -WorldPos0.x));
		
// 		//sun.alt = 4.0 * -sun.ad + 1.6 * PI_4 * (0.5 + cos(0.46 * iGlobalTime) / 2.0);
// 		//sun.alt = 0.3;

// 		vec3 gp = vec3(0.0, earth_rs + 1.0, 0.0);
// 		vec3 res = compute_sky_light(gp, rd, sun);
		
// 		//================================
// 		// POST EFFECTS
		
// 		// if(q.x > 0.0 )
// 		// {
// 			float crush = 0.6;
// 			float frange = 7.9;
// 			float exposure = 48.0;
// 			res = log2(1.0+res*exposure);
// 			res = smoothstep(crush, frange, res);
// 			res = res*res*res*(res*(res*6.0 - 15.0) + 10.0);
// 		// }
		
// 		// if (q.y < 0.5 && q.y > 0.498)
// 		// 	res = vec3(0.0);
		
// 		// switch to gamma space before perceptual tweaks
// 		res = linear_to_gamma(res);
		
// 		// // vignetting
// 		// // tensor product of the parametric curve defined by (4(t-t?2))^0.1
// 		// res *= 0.5 + 0.5 * pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.1);
		
// 		FragColor = vec4(res, 1.0);
// 	}
// 	else
// 	{
// 		FragColor = mix(
// 			vec4(14./255., 58./255., 83./255., 1), 
// 			vec4()
// 		);
// 	}
// }
