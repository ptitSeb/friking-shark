//
// Fixed pipeline implementation is based on:
//    3Dlabs GLSL ShaderGen code (http://developer.3dlabs.com)
//    OpenGLShading Language Third Edition
//

uniform int  g_ActiveLights;
uniform vec3 g_vHeightFogMins;
uniform vec3 g_vHeightFogMaxs;
uniform mat4 CameraModelViewInverse;
varying vec3 g_WorldVertexPos;
varying vec4 g_EyeVertexPos;

#ifdef ENABLE_SKY_SHADOW
uniform vec4 SkyData;
#endif
#ifdef ENABLE_LIGHTING
#ifdef ENABLE_NORMAL_MAP
	varying vec3 g_TangentSpaceX;
	varying vec3 g_TangentSpaceY;
	varying vec3 g_TangentSpaceZ;
#else
	varying vec3 g_normal; 
#endif
#endif
	
#ifdef ENABLE_FOG
varying float g_fFogFactor;
#endif 

void GetTagent(const in vec3 normal,
			   out    vec3 tangent)
{
	vec3 c1 = cross(normal, vec3(0.0, 0.0, 1.0)); 
	vec3 c2 = cross(normal, vec3(0.0, 1.0, 0.0)); 
	tangent = normalize((length(c1)>length(c2))?c1:c2);
}

void main (void)
{
	vec4 LocalVertexPos=gl_Vertex;
	g_EyeVertexPos=gl_ModelViewMatrix * LocalVertexPos;
	vec4 WorldVertexPos=CameraModelViewInverse * g_EyeVertexPos;
	g_WorldVertexPos=WorldVertexPos.xyz;

	#ifdef ENABLE_LIGHTING
	#ifdef ENABLE_NORMAL_MAP
	vec3 vTangent;
	vec3 g_normal=normalize(gl_NormalMatrix * gl_Normal);
	GetTagent(gl_Normal,vTangent);
	vec3 n = g_normal;
	vec3 t = normalize(gl_NormalMatrix * vTangent);
	vec3 b = cross(n, t);
	g_TangentSpaceX=t;
	g_TangentSpaceY=b;
	g_TangentSpaceZ=n;
	#else
	g_normal=normalize(gl_NormalMatrix * gl_Normal);
	#endif 
	#endif
	
	gl_Position = ftransform();

#ifdef ENABLE_FOG
	float fFogSize=g_vHeightFogMaxs.y-g_vHeightFogMins.y;
	float fFogDist=WorldVertexPos.y-g_vHeightFogMins.y;
	g_fFogFactor=1.0-fFogDist/fFogSize;
 	g_fFogFactor*=1.0-step(g_vHeightFogMaxs.x,WorldVertexPos.x);
	g_fFogFactor*=step(g_vHeightFogMins.x,WorldVertexPos.x);
	g_fFogFactor=clamp(g_fFogFactor,0.0,1.0);
#endif
	
	gl_FrontColor=gl_Color;
	
#ifdef ENABLE_TEXTURES
	gl_TexCoord[0]=gl_TextureMatrix[0]*gl_MultiTexCoord0;
	#if TEXTURE_UNITS > 1
	gl_TexCoord[1]=gl_TextureMatrix[1]*gl_MultiTexCoord1;
	#endif
#endif
	
#ifdef ENABLE_NORMAL_MAP
	gl_TexCoord[NORMAL_MAP_TEXTURE_LEVEL]=gl_TextureMatrix[NORMAL_MAP_TEXTURE_LEVEL]*gl_MultiTexCoord2;
#endif
	
#ifdef ENABLE_SKY_SHADOW
	gl_TexCoord[SKY_TEXTURE_LEVEL].s = (g_WorldVertexPos.x/SkyData.y)+SkyData.x;
	gl_TexCoord[SKY_TEXTURE_LEVEL].t = (g_WorldVertexPos.z/SkyData.z);
	gl_TexCoord[SKY_TEXTURE_LEVEL].p = 0.0;
	gl_TexCoord[SKY_TEXTURE_LEVEL].q = 0.0;
#endif
	
#ifdef ENABLE_SHADOWS
	gl_TexCoord[SHADOW_TEXTURE_LEVEL].s = dot(g_EyeVertexPos, gl_EyePlaneS[SHADOW_TEXTURE_LEVEL]);
	gl_TexCoord[SHADOW_TEXTURE_LEVEL].t = dot(g_EyeVertexPos, gl_EyePlaneT[SHADOW_TEXTURE_LEVEL]);
	gl_TexCoord[SHADOW_TEXTURE_LEVEL].p = dot(g_EyeVertexPos, gl_EyePlaneR[SHADOW_TEXTURE_LEVEL]);
	gl_TexCoord[SHADOW_TEXTURE_LEVEL].q = dot(g_EyeVertexPos, gl_EyePlaneQ[SHADOW_TEXTURE_LEVEL]);
#endif
}