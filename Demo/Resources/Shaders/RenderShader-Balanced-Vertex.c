
uniform int  g_ActiveLights;
uniform vec3 g_vHeightFogMins;
uniform vec3 g_vHeightFogMaxs;
uniform mat4 CameraModelViewInverse;
varying vec3 g_WorldVertexPos;
varying vec4 g_EyeVertexPos;
#ifdef ENABLE_LIGHTING
varying vec3 g_normal; 
varying vec4 g_ambdiffspec;

void PointLight(const in int  i,
				const in vec3 ecPosition3,
				const in vec3 normal,
				inout    vec4 ambient,
				inout    vec4 diffuse,
				inout    vec4 specular)
{
	// Compute distance between surface and light position
	float d = length(gl_LightSource[i].position.xyz - ecPosition3);
	vec3 L = normalize(gl_LightSource[i].position.xyz - ecPosition3);
	float lambertTerm = max(dot(normal,L),0.0);
	float attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +gl_LightSource[i].linearAttenuation * d +gl_LightSource[i].quadraticAttenuation * d * d);
	ambient+= gl_LightSource[i].ambient*lambertTerm * attenuation;
	diffuse +=gl_LightSource[i].diffuse*lambertTerm* attenuation;
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


void main (void)
{
	#ifdef ENABLE_LIGHTING
	g_normal=normalize(gl_NormalMatrix * gl_Normal);
	#endif 
	vec4 LocalVertexPos=gl_Vertex;
	g_EyeVertexPos=gl_ModelViewMatrix * LocalVertexPos;
	vec4 WorldVertexPos=CameraModelViewInverse * g_EyeVertexPos;
	g_WorldVertexPos=WorldVertexPos.xyz;

	vec4 amb=vec4(0);
	vec4 diff=vec4(0);
	vec4 spec=vec4(0);
	gl_Position = ftransform();
	#ifdef ENABLE_LIGHTING
	for(int x=1;x<g_ActiveLights;x++)
	{
		PointLight(x, g_EyeVertexPos.xyz, g_normal, amb, diff, spec);
	}
	g_ambdiffspec=gl_LightModel.ambient+amb+diff+spec*gl_FrontMaterial.specular;
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