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
#ifdef ENABLE_LIGHTING
varying vec4 g_ambdiffspec;
varying vec4 g_sunambdiffspec;
#endif
#ifdef ENABLE_FOG
varying float g_fFogFactor;
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

void PointLight(const in int  i,
				const in vec3 ecPosition3,
				const in vec3 normal,
				inout    vec4 ambient,
				inout    vec4 diffuse,
				inout    vec4 specular)
{
	float nDotVP;       // normal . light direction
	float nDotHV; // normal . light half vector	
	float attenuation;  // computed attenuation factor
	float pf; // power factor
	float d;            // distance from surface to light source
	vec3  VP;           // direction from surface to light position
	// Compute vector from surface to 	light position
	VP = vec3(gl_LightSource[i].position) - ecPosition3;
	// Compute distance between surface and light position
	d = length(VP);
	// Normalize the vector from surface to light position
	VP = normalize(VP);
	// Compute attenuation
		
	attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +gl_LightSource[i].linearAttenuation * d +gl_LightSource[i].quadraticAttenuation * d * d);
	nDotVP = max(0.0, dot(normal, VP));
	nDotHV = max(0.0, dot(normal, vec3(gl_LightSource[i].halfVector)));
	pf = max(0.0,pow(nDotHV, gl_FrontMaterial.shininess));
	ambient  += gl_LightSource[i].ambient * attenuation;
	diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;
	specular += gl_LightSource[i].specular * pf * attenuation;
}

void main (void)
{
	vec3 normal=normalize(gl_NormalMatrix * gl_Normal);

	vec4 LocalVertexPos=gl_Vertex;
	vec4 EyeVertexPos=gl_ModelViewMatrix * LocalVertexPos;
	vec4 WorldVertexPos=CameraModelViewInverse * EyeVertexPos;
	g_WorldVertexPos=WorldVertexPos.xyz;

    gl_Position = ftransform();
#ifdef ENABLE_LIGHTING
	vec4 amb=vec4(0);
	vec4 diff=vec4(0);
	vec4 sunamb=vec4(0);
	vec4 sundiff=vec4(0);
	vec4 spec=vec4(0);
	vec4 sunspec=vec4(0);
	
	DirectionalLight(0, normal, sunamb, sundiff,sunspec);
	for(int x=1;x<g_ActiveLights;x++)
	{
		PointLight(x, EyeVertexPos.xyz, normal, amb, diff, spec);
	}
	
	g_ambdiffspec=gl_LightModel.ambient+amb+diff+spec*gl_FrontMaterial.specular;
	g_sunambdiffspec=sunamb+sundiff+sunspec*gl_FrontMaterial.specular;	
	
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
	gl_TexCoord[3].s = dot(EyeVertexPos, gl_EyePlaneS[3]);
	gl_TexCoord[3].t = dot(EyeVertexPos, gl_EyePlaneT[3]);
	gl_TexCoord[3].p = dot(EyeVertexPos, gl_EyePlaneR[3]);
	gl_TexCoord[3].q = dot(EyeVertexPos, gl_EyePlaneQ[3]);
#endif
}