#version 150 

uniform mat4 uView;
uniform mat4 uModel[MAX_OBJECT_INSTANCES];
uniform vec3 uTinting[MAX_OBJECT_INSTANCES];
uniform mat4 uProjection;

uniform mat4 uTexMatrix0;
uniform mat4 uTexMatrix1;
uniform vec4 uMaterialDiffuse;
uniform vec4 uMaterialSpecular;
uniform float uMaterialShininess;
uniform float uDisableVertexColor;

in vec3 aVertex;
in vec2 aTexCoord0;
in vec2 aTexCoord1;
in vec4 aVertexColor;
in vec3 aNormal;
in vec2 aNorCoord;
in vec3 aTangent;
in vec3 aBitangent;

out vec3 g_EyeNormal;
out vec4 g_Color;
out vec2 g_TexCoord0;
out vec2 g_TexCoord1;
out vec2 g_SkyCoord;

#ifdef ENABLE_FOG
uniform vec3 uFogMins;
uniform vec3 uFogMaxs;
out float g_fFogFactor;
#endif
#ifdef ENABLE_SKY_SHADOW
uniform vec4 SkyData;
#endif
#ifdef ENABLE_SHADOWS
uniform mat4 uShadowMatrix;
out vec4 g_ShadowCoord;
#endif

#ifdef ENABLE_NORMAL_MAP
out vec3 g_EyeTangent;
out vec3 g_EyeBitangent;
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
	vec4 worldVertexPos=uModel[gl_InstanceID]*vec4(aVertex,1.0);
	vec4 eyeVertexPos=uView*worldVertexPos;
	mat4 modelview=uView*uModel[gl_InstanceID];
	
	#ifdef ENABLE_NORMAL_MAP
	g_EyeTangent=(modelview*vec4(aTangent,0.0)).xyz;
	g_EyeBitangent=(modelview*vec4(aBitangent,0.0)).xyz;
	#endif	
	#ifdef ENABLE_LIGHTING
	g_EyeNormal=(modelview*vec4(aNormal,0.0)).xyz;
	g_Color=mix(aVertexColor,vec4(1.0),uDisableVertexColor);
	#else
	g_Color=mix(aVertexColor,uMaterialDiffuse,uDisableVertexColor);
	#endif
	g_Color*=vec4(uTinting[gl_InstanceID],1.0);
	
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
	g_TexCoord0=(uTexMatrix0*vec4(aTexCoord0,0.0,1.0)).xy;
	#if TEXTURE_UNITS > 1
	g_TexCoord1=(uTexMatrix1*vec4(aTexCoord1,0.0,1.0)).xy;
	#endif
	#endif
	
	#ifdef ENABLE_SKY_SHADOW
	g_SkyCoord.s = (worldVertexPos.x/SkyData.y)+SkyData.x;
	g_SkyCoord.t = (worldVertexPos.z/SkyData.z);
	#endif
	
	#ifdef ENABLE_SHADOWS
	g_ShadowCoord=uShadowMatrix*worldVertexPos;
	#endif
}
