#version 330 

in vec2 gTexCoord;
in vec3 gPositionMin;
in vec3 gPositionMax;
out vec4 oColor;

uniform sampler2DShadow DepthSampler;
uniform sampler2D NormalSampler;
uniform sampler2D DiffuseSampler;
uniform sampler2DShadow ShadowMap;

uniform mat4 uShadowMatrix;
uniform mat4 uCameraInverse;
uniform float uCameraFar;
uniform float uCameraNear;
uniform float uWidth;
uniform float uHeight;

uniform vec3 uWorldEyeDir;
uniform vec4 uAmbient;
uniform vec4 uSunDiffuse;
uniform vec4 uSunSpecular;
uniform vec3 uSunDirection;
uniform vec3 uSunHalfVector;

vec3 ComputePosition(in vec2 screenCoords,in float fZ)
{
	vec4 vNDCPos=vec4(screenCoords.x*2.0/uWidth-1.0,screenCoords.y*2.0/uHeight-1.0,fZ*2.0-1.0,1.0);
	vec4 vClipPos=vNDCPos*(uCameraFar-uCameraNear);
	vec4 position4=uCameraInverse*vClipPos;
	return position4.xyz/position4.w;
}

void SunLight(const in vec3 normal,inout vec4 diffuse,inout vec4 specular)
{
	diffuse += uSunDiffuse * max(0.0, dot(normal,uSunDirection));
	float nDotHV = max(0.0, dot(normal, uSunHalfVector));
	specular += uSunSpecular * max(0.0,pow(nDotHV, 96.0));
}

void main(void)
{
	float fZ=textureProj(DepthSampler,vec4(gTexCoord,0.0,1.0));
	vec3 position=ComputePosition(gTexCoord,fZ);
	vec4 normalAndShin=texture2D(NormalSampler,gTexCoord);
	vec3 normal=normalize((normalAndShin.xyz-vec3(0.5))*2.0);
	vec3 diffuse=texture2D(DiffuseSampler,gTexCoord).rgb;
	vec3 specular=vec3(normalAndShin.a);

	vec4 sundiff=vec4(0);
	vec4 sunspec=vec4(0);

	SunLight(normal, sundiff,sunspec);
	
	vec4 shadowCoord=uShadowMatrix*vec4(position,1.0);
	float ndc=shadowCoord.z/shadowCoord.w;
	float fShadowFactor=step(ndc,textureProj(ShadowMap,shadowCoord));
	float offset=3.0;
	fShadowFactor+=step(ndc,textureProj(ShadowMap,shadowCoord+vec4(-offset,-offset,0,0)));
	fShadowFactor+=step(ndc,textureProj(ShadowMap,shadowCoord+vec4(offset,-offset,0,0)));
	fShadowFactor+=step(ndc,textureProj(ShadowMap,shadowCoord+vec4(-offset,offset,0,0)));
	fShadowFactor+=step(ndc,textureProj(ShadowMap,shadowCoord+vec4(offset,offset,0,0)));
	fShadowFactor/=5.0;

	gl_FragDepth=fZ;//0.5+vClipPos.z/vClipPos.w*0.5;
	
	
	oColor.rgb=diffuse.rgb*(uAmbient.rgb+sundiff.rgb*fShadowFactor)+specular*(sunspec.rgb*fShadowFactor);
	oColor.a=1.0;
}
