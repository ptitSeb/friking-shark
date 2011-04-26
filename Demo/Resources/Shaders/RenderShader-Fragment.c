//
// Fixed pipeline implementation is based on:
//    3Dlabs GLSL ShaderGen code (http://developer.3dlabs.com)
//    OpenGLShading Language Third Edition
//
// Ripple effect is based on:
//    JeGX's post at http://www.geeks3d.com/20110316/shader-library-simple-2d-effects-sphere-and-ripple-in-glsl/
//    Adrian Boeing's post at http://adrianboeing.blogspot.com/2011/02/ripple-effect-in-webgl.html

uniform mat4 CameraModelViewInverse;
uniform float CurrentRealTime;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2DShadow ShadowMap;

#define LIGHTING_SATURATION 1.5

varying vec3 g_WorldVertexPos;
#ifdef ENABLE_LIGHTING
varying vec4 g_amb;
varying vec4 g_diff;
varying vec4 g_spec;
varying vec4 g_sunamb;
varying vec4 g_sundiff;
varying vec4 g_sunspec;
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
  
  #ifdef ENABLE_LIGHTING
	  vec4 amb=g_amb;
	  vec4 diff=g_diff;
	  vec4 spec=g_spec;
  #endif
  
  #ifdef ENABLE_SHADOWS
	  
	  fShadowFactor=shadow2DProj(ShadowMap, gl_TexCoord[3]).r;
  #endif
  #ifdef ENABLE_LIGHTING
	  amb+=g_sunamb*fShadowFactor;
	  diff+=g_sundiff*fShadowFactor;
	  spec+=g_sunspec*fShadowFactor;
	  finalcolor=(clamp(gl_LightModel.ambient+amb+diff,0.0,LIGHTING_SATURATION)*texcolor)+(spec*gl_FrontMaterial.specular*texcolor);
	  finalcolor.a=texcolor.a;
  #else
	finalcolor=texcolor*fShadowFactor;
  #endif
	
  
  #ifdef ENABLE_FOG
	finalcolor= vec4(clamp(finalcolor.rgb, 0.0, 1.0),finalcolor.a);
	finalcolor.rgb=mix(gl_Fog.color.rgb,finalcolor.rgb,g_fFogFactor);
  #endif

	gl_FragColor=finalcolor;
}