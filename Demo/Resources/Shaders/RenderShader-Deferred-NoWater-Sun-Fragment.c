#version 150 

in vec2 gTexCoord;
in vec3 gPositionMin;
in vec3 gPositionMax;
out vec4 oColor;

uniform sampler2DShadow DepthSampler;
uniform sampler2D NormalSampler;
uniform sampler2D DiffuseSampler;
uniform sampler2DShadow ShadowMap;

uniform mat4 uShadowMatrix;
uniform mat4 uUnprojectMatrix;

uniform vec4 uAmbient;
uniform vec4 uSunDiffuse;
uniform vec4 uSunSpecular;
uniform vec3 uSunDirection;
uniform vec3 uSunHalfVector;

uniform sampler2D SkyShadowMap;
uniform vec4 uSkyData;

vec3 ComputePosition(in vec2 screenCoords,in float fZ)
{
	vec4 position4=uUnprojectMatrix*vec4(screenCoords.x,screenCoords.y,fZ,1.0);
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

	// pixels with depth=1 are very likely to come from a clear
	// operation, to avoid clearing the normal buffer each frame
	// we deactivate specular component if z=1.
	// pixels not drawn on this frame will have garbage normal and z=1
	// this is a lot faster than clearing the normal buffer each frame
	
	float fDeactivateGarbageNormal=(1.0-trunc(fZ));
	vec3 specular=vec3(normalAndShin.a*fDeactivateGarbageNormal);

	vec4 sundiff=vec4(0);
	vec4 sunspec=vec4(0);

	SunLight(normal, sundiff,sunspec);
	
	float fShadowFactor=1.0;
	vec4 shadowCoord=uShadowMatrix*vec4(position,1.0);
	float ndc=shadowCoord.z/shadowCoord.w;
	fShadowFactor=step(ndc,textureProj(ShadowMap,shadowCoord));
	float offset=3.0;
	fShadowFactor+=step(ndc,textureProj(ShadowMap,shadowCoord+vec4(-offset,-offset,0,0)));
	fShadowFactor+=step(ndc,textureProj(ShadowMap,shadowCoord+vec4(offset,-offset,0,0)));
	fShadowFactor+=step(ndc,textureProj(ShadowMap,shadowCoord+vec4(-offset,offset,0,0)));
	fShadowFactor+=step(ndc,textureProj(ShadowMap,shadowCoord+vec4(offset,offset,0,0)));
	fShadowFactor/=5.0;
	
	vec2 skyCoord;
	skyCoord.s = (position.x/uSkyData.y)+uSkyData.x;
	skyCoord.t = (position.z/uSkyData.z);
	
	float skyshadowfactor=1.0-(texture2D(SkyShadowMap, skyCoord).r*uSkyData.a);
	fShadowFactor*=skyshadowfactor;
	
	gl_FragDepth=fZ;
	
	oColor.rgb=diffuse.rgb*(uAmbient.rgb+sundiff.rgb*fShadowFactor)+specular*(sunspec.rgb*fShadowFactor);
	oColor.a=1.0;
}
