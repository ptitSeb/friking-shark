precision highp float;

uniform float CurrentRealTime;
uniform vec3 uFogColor;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

varying vec4 g_Color;
varying vec2 g_TexCoord0;
varying vec2 g_TexCoord1;

#ifdef ENABLE_SHADOWS
uniform sampler2D ShadowMap;
varying vec4 g_ShadowCoord;
#endif

#ifdef ENABLE_LIGHTING

varying vec3 g_DiffuseSpecular;
varying vec3 g_Ambient;

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
	
	vec3 texcolor=texture2D(sampler, uv).xyz;
	color.rgb = texcolor;
	color.rgb+=texcolor.rgb*max(0.0,ripplesize)*0.05;
	color.rgb+=texcolor.rgb*max(0.0,ripplesize2)*0.10;	
}
#endif

void main (void) 
{
  vec4 texcolor=g_Color;
  vec4 finalcolor;
  float fShadowFactor=1.0;
  
#ifdef ENABLE_TEXTURES
  #ifdef ENABLE_WATER
	ApplyWaterEffect(Texture0,g_TexCoord0.xy,texcolor.xyz);
  #else
    texcolor*= texture2D(Texture0, g_TexCoord0.xy);
  #endif
#endif
  
  #ifdef ENABLE_SHADOWS
	float ndc=g_ShadowCoord.z/g_ShadowCoord.w;
	fShadowFactor=step(ndc,texture2DProj(ShadowMap,g_ShadowCoord).r);
  #endif

#ifdef ENABLE_LIGHTING
	  
	finalcolor.rgb=texcolor.rgb*(g_Ambient+g_DiffuseSpecular*fShadowFactor);
	finalcolor.a=texcolor.a;
#else

 	finalcolor.rgb=texcolor.rgb*fShadowFactor;
	finalcolor.a=texcolor.a;
#endif

#ifdef ENABLE_FOG
	finalcolor= vec4(clamp(finalcolor.rgb, 0.0, 1.0),finalcolor.a);
	finalcolor.rgb=mix(finalcolor.rgb,uFogColor,g_fFogFactor);
#endif

	gl_FragColor=finalcolor;
}
