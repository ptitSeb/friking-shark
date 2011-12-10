#version 150

uniform float CurrentRealTime;
uniform vec3 uFogColor;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

in vec4 g_Color;
in vec2 g_TexCoord0;
in vec2 g_TexCoord1;
in vec2 g_SkyCoord;

out vec4 oNormal;
out vec4 oDiffuse;

#ifdef ENABLE_SHADOWS
uniform sampler2DShadow ShadowMap;
in vec4 g_ShadowCoord;
#endif
#ifdef ENABLE_NORMAL_MAP
in vec3  g_EyeTangent;
in vec3  g_EyeBitangent;
uniform mat4 uModel[MAX_OBJECT_INSTANCES];
uniform sampler2D NormalMap;
#endif
#ifdef ENABLE_SKY_SHADOW
uniform sampler2D SkyShadowMap;
uniform vec4 SkyData;
#endif


uniform vec4 uMaterialSpecular;
in vec3 g_EyeNormal;

uniform vec3 uWorldEyeDir;
uniform float uMaterialShininess;
uniform vec4 uMaterialDiffuse;

#ifdef ENABLE_FOG
in float g_fFogFactor;
#endif 


#ifdef ENABLE_WATER
uniform vec2 WaterMappingSize;
uniform vec2 WaterMappingOffset;

void ApplyWaterEffect(in sampler2D sampler,in vec2 vCoords, out vec3 color,inout vec4 normalDisturbance)
{
	// Ripple effect, based on JeGX's post at http://www.geeks3d.com/20110316/shader-library-simple-2d-effects-sphere-and-ripple-in-glsl/
	// and Adrian Boeing's post at http://adrianboeing.blogspot.com/2011/02/ripple-effect-in-webgl.html
	
	vec2 tc = vec2((WaterMappingOffset.x-WaterMappingSize.x)-g_TexCoord0.x,WaterMappingSize.y*4.0-g_TexCoord0.y);
	vec2 tc2= vec2((WaterMappingOffset.x-WaterMappingSize.x*0.5)-g_TexCoord0.x,-g_TexCoord0.y);
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
	
	vec4 tempdisturb=vec4(0.0);
	tempdisturb.xy=vec2(cos((len2*3.0+CurrentRealTime*1.5)*2.0)-cos((len2*3.1+CurrentRealTime*1.5)*4.0));
	tempdisturb.xy+=vec2(cos((len*3.0+CurrentRealTime)*2.0)+cos((len*3.1+CurrentRealTime)*4.0));
	tempdisturb.z=1.0;
	normalDisturbance+=normalize(tempdisturb)*0.6;
	
	vec3 texcolor=texture2D(sampler, uv).xyz;
	color.rgb = texcolor;
	color.rgb+=texcolor.rgb*max(0.0,ripplesize)*0.075;
	color.rgb+=texcolor.rgb*max(0.0,ripplesize2)*0.15;	
}
#endif

void main (void) 
{
  vec4 texcolor=g_Color;
  vec4 finalcolor;
  vec4 normalDisturbance=vec4(0);
  float fShadowFactor=1.0;  

#ifdef ENABLE_TEXTURES
  #ifdef ENABLE_WATER
  ApplyWaterEffect(Texture0,g_TexCoord0.xy,texcolor.xyz,normalDisturbance);
  #else
    texcolor*= texture2D(Texture0, g_TexCoord0.xy);
  #endif
	#if TEXTURE_UNITS > 1
		#ifdef ENABLE_WATER
			vec3 temptexunit1;
			ApplyWaterEffect(Texture1,g_TexCoord1.xy,temptexunit1);
			texcolor.rgb*=temptexunit1;
		#else
		texcolor*= texture2D(Texture1, g_TexCoord1.xy);
		#endif
	#endif
#endif
		
 #ifdef ENABLE_NORMAL_MAP
	  // Compute final normal usign the normal map
	  vec4 normalMapSample=texture2D(NormalMap,g_TexCoord0.xy);
	  vec3 bump = vec3(normalMapSample.xy* 2.0 - vec2(1.0),normalMapSample.z);
	  vec3 N=normalize(bump.x*normalize(g_EyeBitangent)+bump.y*normalize(g_EyeTangent)+bump.z*normalize(g_EyeNormal));
	 // ks=normalMapSample.a;
#else
	  vec3 N=normalize(g_EyeNormal/*+normalDisturbance.xyz*/);
#endif
		
#ifdef ENABLE_LIGHTING
	finalcolor=texcolor*uMaterialDiffuse;
#else
	finalcolor=texcolor;
#endif
	
  #ifdef ENABLE_FOG
	finalcolor= vec4(clamp(finalcolor.rgb, 0.0, 1.0),finalcolor.a);
	finalcolor.rgb=mix(finalcolor.rgb,uFogColor,g_fFogFactor);
  #endif

	oNormal=vec4((N.xy*0.5)+vec2(0.5),uMaterialSpecular.r,finalcolor.a);
	oDiffuse=finalcolor;
}
