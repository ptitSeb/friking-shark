//
// Fixed pipeline implementation is based on:
//    3Dlabs GLSL ShaderGen code (http://developer.3dlabs.com)
//    OpenGLShading Language Third Edition
//

#define MAX_LIGHTS 8

uniform int  g_ActiveLights;
uniform vec3 g_vHeightFogMins;
uniform vec3 g_vHeightFogMaxs;
uniform mat4 CameraModelViewInverse;
varying vec3 g_WorldVertexPos;
varying vec4 g_EyeVertexPos;
#ifdef ENABLE_LIGHTING
varying vec3 g_normal; 
varying vec3 g_lightdirs[MAX_LIGHTS];
#endif
#ifdef ENABLE_FOG
varying float g_fFogFactor;
#endif 


void main (void)
{
	#ifdef ENABLE_LIGHTING
	g_normal=normalize(gl_NormalMatrix * gl_Normal);
	#endif 
	vec4 LocalVertexPos=gl_Vertex;
	g_EyeVertexPos=gl_ModelViewMatrix * LocalVertexPos;
	vec4 WorldVertexPos=CameraModelViewInverse * g_EyeVertexPos;
	g_WorldVertexPos=WorldVertexPos.xyz;

	gl_Position = ftransform();
	#ifdef ENABLE_LIGHTING
	for (int i=0; i<MAX_LIGHTS; ++i)
	{
		g_lightdirs[i] =vec3(gl_LightSource[i].position.xyz - g_EyeVertexPos.xyz);
	}
	#endif

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
	#if TEXTURE_UNITS > 2
	gl_TexCoord[2]=gl_TextureMatrix[2]*gl_MultiTexCoord2;
	#endif
#endif

#ifdef ENABLE_SHADOWS
	gl_TexCoord[3].s = dot(g_EyeVertexPos, gl_EyePlaneS[3]);
	gl_TexCoord[3].t = dot(g_EyeVertexPos, gl_EyePlaneT[3]);
	gl_TexCoord[3].p = dot(g_EyeVertexPos, gl_EyePlaneR[3]);
	gl_TexCoord[3].q = dot(g_EyeVertexPos, gl_EyePlaneQ[3]);
#endif
}