//
// Fixed pipeline implementation is based on:
//    3Dlabs GLSL ShaderGen code (http://developer.3dlabs.com)
//    OpenGLShading Language Third Edition
//
// Ripple effect is based on:
//    JeGX's post at http://www.geeks3d.com/20110316/shader-library-simple-2d-effects-sphere-and-ripple-in-glsl/
//    Adrian Boeing's post at http://adrianboeing.blogspot.com/2011/02/ripple-effect-in-webgl.html

uniform int  g_ActiveLights;
uniform mat4 CameraModelViewInverse;
uniform float CurrentRealTime;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
#ifdef ENABLE_SHADOWS
uniform sampler2DShadow ShadowMap;
#endif
#ifdef ENABLE_NORMAL_MAP
uniform sampler2D NormalMap;
#endif
#ifdef ENABLE_SKY_SHADOW
uniform sampler2D SkyShadowMap;
uniform vec4 SkyData;
#endif

varying vec4 g_EyeVertexPos;

#define LIGHTING_SATURATION 1.5

varying vec3 g_WorldVertexPos;
#ifdef ENABLE_LIGHTING
varying vec4 g_ambdiffspec;
#ifdef ENABLE_NORMAL_MAP
	varying vec3 g_TangentSpaceX;
	varying vec3 g_TangentSpaceY;
	varying vec3 g_TangentSpaceZ;
#else
	varying vec3 g_normal;
#endif
	
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
#endif
			
  #ifdef ENABLE_SHADOWS
	fShadowFactor=shadow2DProj(ShadowMap,gl_TexCoord[SHADOW_TEXTURE_LEVEL]).g;
	#ifdef ENABLE_SOFT_SHADOWS
		float offset=3.0;
		fShadowFactor+=shadow2DProj(ShadowMap,gl_TexCoord[SHADOW_TEXTURE_LEVEL]+vec4(-offset,-offset,0,0)).g;
		fShadowFactor+=shadow2DProj(ShadowMap,gl_TexCoord[SHADOW_TEXTURE_LEVEL]+vec4(offset,-offset,0,0)).g;
		fShadowFactor+=shadow2DProj(ShadowMap,gl_TexCoord[SHADOW_TEXTURE_LEVEL]+vec4(-offset,offset,0,0)).g;
		fShadowFactor+=shadow2DProj(ShadowMap,gl_TexCoord[SHADOW_TEXTURE_LEVEL]+vec4(offset,offset,0,0)).g;
		fShadowFactor/=5.0;
	#endif
  #endif
  #ifdef ENABLE_LIGHTING
	  
	  vec4 sunamb=vec4(0);
	  vec4 sundiff=vec4(0);
	  vec4 sunspec=vec4(0);
	  vec4 g_sunambdiffspec=vec4(0);
	  
	  #ifdef ENABLE_NORMAL_MAP
	  // Compute final normal usign the normal map
	  vec3 bump = normalize( texture2D(NormalMap, gl_TexCoord[NORMAL_MAP_TEXTURE_LEVEL].xy).xyz * 2.0 - 1.0);
	  vec3 N=normalize(bump.x*normalize(g_TangentSpaceY)+bump.y*normalize(g_TangentSpaceX)+bump.z*normalize(g_TangentSpaceZ));
	  #else
	  vec3 N=normalize(g_normal);
	  #endif
	  	  
	  DirectionalLight(0, N, sunamb, sundiff,sunspec);
	  g_sunambdiffspec=sunamb+sundiff+sunspec*gl_FrontMaterial.specular;	
	  
	  #ifdef ENABLE_SKY_SHADOW
		g_sunambdiffspec*=1.0-(texture2D(SkyShadowMap, gl_TexCoord[SKY_TEXTURE_LEVEL].xy)*SkyData.a);
	  #endif	
	  
	  finalcolor.rgb=clamp(g_ambdiffspec.rgb+g_sunambdiffspec.rgb*fShadowFactor,0.0,LIGHTING_SATURATION);
	  finalcolor.rgb*=texcolor.rgb;
	  finalcolor.a=texcolor.a;
  #else
	#ifdef ENABLE_SKY_SHADOW
	  fShadowFactor*=1.0-(texture2D(SkyShadowMap, gl_TexCoord[SKY_TEXTURE_LEVEL].xy)*SkyData.a).r;
	#endif	
	finalcolor=texcolor*fShadowFactor;
  #endif
	
  
  #ifdef ENABLE_FOG
	finalcolor= vec4(clamp(finalcolor.rgb, 0.0, 1.0),finalcolor.a);
	finalcolor.rgb=mix(finalcolor.rgb,gl_Fog.color.rgb,g_fFogFactor);
  #endif

	gl_FragColor=finalcolor;
}