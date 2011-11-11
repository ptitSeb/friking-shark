precision highp float;

uniform mat4 uView;
//uniform mat4 uModel[MAX_OBJECT_INSTANCES];
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
attribute vec2 aNorCoord;
attribute vec3 aTangent;
attribute vec3 aBitangent;

varying vec4 g_WorldVertexPos;
varying vec4 g_EyeVertexPos;
varying vec4 g_Color;
varying vec2 g_TexCoord0;
varying vec2 g_TexCoord1;
varying vec2 g_SkyCoord;

#ifdef ENABLE_FOG
uniform vec3 uFogMins;
uniform vec3 uFogMaxs;
varying float g_fFogFactor;
#endif

#ifdef ENABLE_SKY_SHADOW
uniform vec4 SkyData;
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

varying vec3 g_WorldNormal; 

varying vec4 sundiff;
varying vec4 sunspec;

void SunLight(const in vec3 normal,inout vec4 diffuse,inout vec4 specular)
{
	diffuse += uSunDiffuse * max(0.0, dot(normal,uSunDirection));
	vec3 hv=normalize(uSunDirection-uWorldEyeDir);
	float nDotHV = max(0.0, dot(normal, hv));
	specular += uSunSpecular * max(0.0,pow(nDotHV, uMaterialShininess));
}

#endif
/*
#ifdef ENABLE_NORMAL_MAP
varying vec3 g_WorldTangent;
varying vec3 g_WorldBitangent;
#endif
void GetTagent(const in vec3 normal,
			   out    vec3 tangent)
{
	vec3 c1 = cross(normal, vec3(0.0, 0.0, 1.0)); 
	vec3 c2 = cross(normal, vec3(0.0, 1.0, 0.0)); 
	tangent = normalize((length(c1)>length(c2))?c1:c2);
}*/

void main (void)
{
	g_WorldVertexPos=uModel*vec4(aVertex,1.0);
	g_EyeVertexPos=uView*g_WorldVertexPos;
/*
	#ifdef ENABLE_NORMAL_MAP
	g_WorldTangent=(uModel*vec4(aTangent,0.0)).xyz;
	g_WorldBitangent=(uModel*vec4(aBitangent,0.0)).xyz;
	#endif	
*/	
	#ifdef ENABLE_LIGHTING
	g_WorldNormal=(uModel*vec4(aNormal,0.0)).xyz;
	g_Color=mix(aVertexColor,vec4(1.0),uDisableVertexColor);
	#else
	g_Color=mix(aVertexColor,uMaterialDiffuse,uDisableVertexColor);
	#endif
	//g_Color=mix(aVertexColor,uMaterialDiffuse,uDisableVertexColor);

	
	gl_Position = uProjection*g_EyeVertexPos;

#ifdef ENABLE_FOG
	float fFogSize=uFogMaxs.y-uFogMins.y;
	float fFogDist=g_WorldVertexPos.y-uFogMins.y;
	g_fFogFactor=1.0-fFogDist/fFogSize;
 	g_fFogFactor*=1.0-step(uFogMaxs.x,g_WorldVertexPos.x);
	g_fFogFactor*=step(uFogMins.x,g_WorldVertexPos.x);
	g_fFogFactor=clamp(g_fFogFactor,0.0,1.0);
#endif
	
	
#ifdef ENABLE_TEXTURES
	g_TexCoord0=(uTexMatrix0*vec4(aTexCoord0,0.0,0.0)).xy;
	#if TEXTURE_UNITS > 1
	g_TexCoord1=(uTexMatrix1*vec4(aTexCoord1,0.0,0.0)).xy;
	#endif
#endif
/*
#ifdef ENABLE_SKY_SHADOW
	g_SkyCoord.s = (g_WorldVertexPos.x/SkyData.y)+SkyData.x;
	g_SkyCoord.t = (g_WorldVertexPos.z/SkyData.z);
#endif
	*/
#ifdef ENABLE_SHADOWS
	g_ShadowCoord=uShadowMatrix*g_WorldVertexPos;
#endif
	
#ifdef ENABLE_LIGHTING
	sundiff=vec4(0);
	sunspec=vec4(0);
	SunLight(g_WorldNormal, sundiff,sunspec);
#endif	
}
