#version 330 

in vec2 gTexCoord;
out vec4 oColor;

#define PI 3.1416

uniform sampler2DShadow DepthSampler;
uniform sampler2D NormalSampler;
uniform sampler2D DiffuseSampler;

uniform mat4 uCameraInverse;
uniform float uCameraFar;
uniform float uCameraNear;
uniform float uWidth;
uniform float uHeight;

uniform vec3 uWorldEyeDir;
uniform float uDynamicRange;
uniform vec3 uDynamicPosition;
uniform vec4 uDynamicDiffuse;
uniform vec4 uDynamicSpecular;

void DynamicLight(const in vec3 worldposition,
				  const in vec3 normal,
				  inout    vec4 diffuse,
				  inout    vec4 specular)
{
	vec3 lightVec=uDynamicPosition.xyz - worldposition.xyz;
	
	float squaredRange=uDynamicRange*uDynamicRange;
	float squareddist=dot(lightVec,lightVec);
	if(squareddist>squaredRange){discard;}
	//:if(squareddist>squaredRange){diffuse.b+=1.0;}
	
	float lambertTerm = dot(normal,lightVec); //  lambertTerm is in fact lambertTerm*distance
	if(lambertTerm<0.0){discard;}
	//diffuse.r+=1.0;
	
	float dist=sqrt(squareddist);
	lambertTerm/=dist;
	lightVec/=dist;
	
	//float fStrength=(1.0-(squareddist/squaredRange));
	//float fStrength=2.0*(1.0-(squareddist/squaredRange));
	float fStrength=3.0*((cos((dist/uDynamicRange)*PI)+1.0)*0.5);
	// Compute distance between surface and light position
	diffuse+=uDynamicDiffuse*lambertTerm*fStrength;
	
	vec3 R = reflect(-lightVec, normal);
	float specularFactor = pow(max(dot(R, -uWorldEyeDir), 0.0),96.0);
	specular += uDynamicSpecular*specularFactor*fStrength;
}

vec3 ComputePosition(in vec2 screenCoords,in float fZ)
{
	vec4 vNDCPos=vec4(screenCoords.x*2.0/uWidth-1.0,screenCoords.y*2.0/uHeight-1.0,fZ*2.0-1.0,1.0);
	vec4 vClipPos=vNDCPos*(uCameraFar-uCameraNear);
	vec4 position4=uCameraInverse*vClipPos;
	return position4.xyz/position4.w;
}
void main(void)
{
	float fZ=textureProj(DepthSampler,vec4(gTexCoord,0.0,1.0));
	vec3 position=ComputePosition(gTexCoord,fZ);
	vec4 normalAndShin=texture2D(NormalSampler,gTexCoord);
	vec3 normal=normalize((normalAndShin.xyz-vec3(0.5))*2.0);
	vec3 diffuse=texture2D(DiffuseSampler,gTexCoord).rgb;
	vec3 specular=vec3(normalAndShin.a);

	vec4 dyndiff=vec4(0);
	vec4 dynspec=vec4(0);

	DynamicLight(position,normal, dyndiff, dynspec);
		
	gl_FragDepth=fZ;
	
	oColor.rgb=diffuse.rgb*(dyndiff.rgb)+specular*(dynspec.rgb);
	oColor.a=1.0;
}
