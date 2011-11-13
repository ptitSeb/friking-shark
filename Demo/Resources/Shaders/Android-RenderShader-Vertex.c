precision highp float;

uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uProjection;

uniform mat4 uTexMatrix0;
uniform mat4 uTexMatrix1;
uniform vec4 uMaterialDiffuse;
uniform float uDisableVertexColor;

attribute vec3 aVertex;
attribute vec2 aTexCoord0;
attribute vec2 aTexCoord1;
attribute vec4 aVertexColor;
attribute vec3 aNormal;

varying vec4 g_Color;
varying vec2 g_TexCoord0;
varying vec2 g_TexCoord1;

#ifdef ENABLE_FOG
uniform vec3 uFogMins;
uniform vec3 uFogMaxs;
varying float g_fFogFactor;
#endif

#ifdef ENABLE_SHADOWS
uniform mat4 uShadowMatrix;
varying vec4 g_ShadowCoord;
#endif

#ifdef ENABLE_LIGHTING

uniform vec3 uWorldEyeDir;
uniform float uMaterialShininess;
uniform vec4 uMaterialSpecular;

uniform vec4 uAmbient;
uniform vec4 uSunDiffuse;
uniform vec4 uSunSpecular;
uniform vec3 uSunDirection;

varying vec3 g_DiffuseSpecular;
varying vec3 g_Ambient;

void SunLight(const in vec3 normal,inout vec4 diffuse,inout vec4 specular)
{
	diffuse += uSunDiffuse * max(0.0, dot(normal,uSunDirection));
	vec3 hv=normalize(uSunDirection-uWorldEyeDir);
	float nDotHV = max(0.0, dot(normal, hv));
	specular += uSunSpecular * max(0.0,pow(nDotHV, uMaterialShininess));
}
#endif

void main (void)
{
	vec4 worldVertexPos=uModel*vec4(aVertex,1.0);
	vec4 eyeVertexPos=uView*worldVertexPos;

	#ifdef ENABLE_LIGHTING
	g_Color=mix(aVertexColor,vec4(1.0),uDisableVertexColor);
	#else
	g_Color=mix(aVertexColor,uMaterialDiffuse,uDisableVertexColor);
	#endif
	
	gl_Position = uProjection*eyeVertexPos;

#ifdef ENABLE_FOG
	float fFogSize=uFogMaxs.y-uFogMins.y;
	float fFogDist=worldVertexPos.y-uFogMins.y;
	g_fFogFactor=1.0-fFogDist/fFogSize;
	g_fFogFactor*=1.0-step(uFogMaxs.x,worldVertexPos.x);
	g_fFogFactor*=step(uFogMins.x,worldVertexPos.x);
	g_fFogFactor=clamp(g_fFogFactor,0.0,1.0);
#endif
	
	
#ifdef ENABLE_TEXTURES
	g_TexCoord0=(uTexMatrix0*vec4(aTexCoord0,0.0,0.0)).xy;
	#if TEXTURE_UNITS > 1
	g_TexCoord1=(uTexMatrix1*vec4(aTexCoord1,0.0,0.0)).xy;
	#endif
#endif
	
#ifdef ENABLE_SHADOWS
	g_ShadowCoord=uShadowMatrix*worldVertexPos;
#endif
	
#ifdef ENABLE_LIGHTING
	vec4 sundiff=vec4(0);
	vec4 sunspec=vec4(0);
	vec3 worldNormal=(uModel*vec4(aNormal,0.0)).xyz;
	SunLight(worldNormal, sundiff,sunspec);

	g_DiffuseSpecular=(uMaterialDiffuse.rgb*sundiff.rgb+uMaterialSpecular.rgb*sunspec.rgb);
	g_Ambient=uMaterialDiffuse.rgb*uAmbient.rgb;

#endif	
}
