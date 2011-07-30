//
// Fixed pipeline implementation is based on:
//    3Dlabs GLSL ShaderGen code (http://developer.3dlabs.com)
//    OpenGLShading Language Third Edition
//
// Ripple effect is based on:
//    JeGX's post at http://www.geeks3d.com/20110316/shader-library-simple-2d-effects-sphere-and-ripple-in-glsl/
//    Adrian Boeing's post at http://adrianboeing.blogspot.com/2011/02/ripple-effect-in-webgl.html

#define MAX_LIGHTS 8

uniform int  g_ActiveLights;
uniform mat4 CameraModelViewInverse;
uniform float CurrentRealTime;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2DShadow ShadowMap;
varying vec4 g_EyeVertexPos;

#define LIGHTING_SATURATION 1.5

varying vec3 g_WorldVertexPos;
#ifdef ENABLE_LIGHTING
varying vec3 g_normal;
varying vec3 g_lightdirs[MAX_LIGHTS];

void DirectionalLight(const in int  i,
					  const in vec3 normal,
					  inout    vec4 ambient,
					  inout    vec4 diffuse,
					  inout    vec4 specular)
{
	float nDotVP; // normal . light direction
	float nDotHV; // normal . light half vector	
	float pf; // power factor
	nDotVP = max(0.0, dot(normal,vec3(gl_LightSource[i].position)));
	nDotHV = max(0.0, dot(normal, vec3(gl_LightSource[i].halfVector)));
	pf = max(0.0,pow(nDotHV, gl_FrontMaterial.shininess));
	ambient  += gl_LightSource[i].ambient;
	diffuse  += gl_LightSource[i].diffuse * nDotVP;
	specular += gl_LightSource[i].specular * pf;
}

void PointLight(const in int  i,
				const in vec3 ecPosition3,
				const in vec3 normal,
				inout    vec4 ambient,
				inout    vec4 diffuse,
				inout    vec4 specular)
{
	// Compute distance between surface and light position
	float d = length(vec3(gl_LightSource[i].position) - ecPosition3);
	vec3 L = normalize(g_lightdirs[i]);
	float attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +gl_LightSource[i].linearAttenuation * d +gl_LightSource[i].quadraticAttenuation * d * d);
	float lambertTerm = max(dot(normal,L),0.0);
	
	ambient+= gl_LightSource[i].ambient*lambertTerm * attenuation;
	diffuse+=gl_LightSource[i].diffuse*lambertTerm* attenuation;
	if(gl_FrontMaterial.shininess>0.0)
	{
		vec3 E = normalize(-ecPosition3);
		vec3 R = reflect(-L, normal);
		float specularFactor = pow(max(dot(R, E), 0.0),gl_FrontMaterial.shininess);
		specular += gl_LightSource[i].specular *gl_FrontMaterial.specular* specularFactor * attenuation;
	}
}
#endif

#ifdef ENABLE_FOG
varying float g_fFogFactor;
#endif 

#ifdef ENABLE_WATER
uniform vec2 WaterMappingSize;
uniform vec2 WaterMappingOffset;

void ApplyWaterEffect(in sampler2D sampler,in vec2 vCoords, out vec3 color)
{
	// Ripple effect, based on JeGX's post at http://www.geeks3d.com/20110316/shader-library-simple-2d-effects-sphere-and-ripple-in-glsl/
	// and Adrian Boeing's post at http://adrianboeing.blogspot.com/2011/02/ripple-effect-in-webgl.html
	
	vec2 tc = vec2((WaterMappingOffset.x-WaterMappingSize.x)-gl_TexCoord[0].x,WaterMappingSize.y*4.0-gl_TexCoord[0].y);
	vec2 tc2= vec2((WaterMappingOffset.x-WaterMappingSize.x*0.5)-gl_TexCoord[0].x,-gl_TexCoord[0].y);
	vec2 p = -1.0 + 2.0*tc;
	vec2 p2= -1.0 + 2.0*tc2;
	
	float len = length(p);
	float len2 = length(p2);
	float ripplesize=cos((len*3.0+CurrentRealTime)*4.0);
	float ripplesize2=cos((len2*3.0+CurrentRealTime*1.5)*4.0);
	vec2 uv=tc;
	uv+=(p/len)*ripplesize*0.03;
	uv+=(p2/len)*ripplesize2*0.03;
	uv-=WaterMappingOffset;
	
	vec3 texcolor=texture2D(sampler, uv).xyz;
	color.rgb = texcolor;
	color.rgb+=texcolor.rgb*max(0.0,ripplesize)*0.05;
	color.rgb+=texcolor.rgb*max(0.0,ripplesize2)*0.10;	
}
#endif

void main (void) 
{
  vec4 texcolor=gl_Color;
  vec4 finalcolor;
  float fShadowFactor=1.0;
  float specfactor=1.0;
  
#ifdef ENABLE_TEXTURES

  #ifdef ENABLE_WATER
  ApplyWaterEffect(Texture0,gl_TexCoord[0].xy,texcolor.xyz);
  #else
    texcolor*= texture2D(Texture0, gl_TexCoord[0].xy);
  #endif
	#if TEXTURE_UNITS > 1
		#ifdef ENABLE_WATER
			vec3 temptexunit1;
			ApplyWaterEffect(Texture1,gl_TexCoord[1].xy,temptexunit1);
			texcolor.rgb*=temptexunit1;
		#else
		texcolor*= texture2D(Texture1, gl_TexCoord[1].xy);
		#endif
	#endif
	#if TEXTURE_UNITS > 2
		#ifdef ENABLE_WATER
			vec3 temptexunit2;
			ApplyWaterEffect(Texture2,gl_TexCoord[2].xy,temptexunit2);
			texcolor.rgb*=temptexunit2;
		#else
			texcolor*= texture2D(Texture2, gl_TexCoord[2].xy);
		#endif
	#endif
#endif
  
  #ifdef ENABLE_SHADOWS
	  
	  fShadowFactor=shadow2DProj(ShadowMap, gl_TexCoord[3]).r;
  #endif
  #ifdef ENABLE_LIGHTING
	  
	  vec4 amb=vec4(0);
	  vec4 diff=vec4(0);
	  vec4 sunamb=vec4(0);
	  vec4 sundiff=vec4(0);
	  vec4 spec=vec4(0);
	  vec4 sunspec=vec4(0);
	  vec4 g_ambdiffspec=vec4(0);
	  vec4 g_sunambdiffspec=vec4(0);
	  vec4 Idiff;
	  vec4 Ispec;
	  
	  vec3 N = normalize(g_normal);
	  
	  DirectionalLight(0, N, sunamb, sundiff,sunspec);
	  for(int x=1;x<g_ActiveLights;x++)
	  {
			PointLight(x, g_EyeVertexPos.xyz, N, amb, diff, spec);
	  }
	  
	  g_ambdiffspec=gl_LightModel.ambient+amb+diff+spec*gl_FrontMaterial.specular;
	  g_sunambdiffspec=sunamb+sundiff+sunspec*gl_FrontMaterial.specular;	
	  
	  finalcolor.rgb=clamp(g_ambdiffspec.rgb+g_sunambdiffspec.rgb*fShadowFactor,0.0,LIGHTING_SATURATION);
	  finalcolor.rgb*=texcolor.rgb;
	  finalcolor.a=texcolor.a;
  #else
	finalcolor=texcolor*fShadowFactor;
  #endif
	
  
  #ifdef ENABLE_FOG
	finalcolor= vec4(clamp(finalcolor.rgb, 0.0, 1.0),finalcolor.a);
	finalcolor.rgb=mix(finalcolor.rgb,gl_Fog.color.rgb,g_fFogFactor);
  #endif

	gl_FragColor=finalcolor;
}