#version 150 

#define PI 3.1416

in vec2 gTexCoord;
out vec4 oColor;

uniform sampler2DShadow DepthSampler;
uniform sampler2D NormalSampler;
uniform sampler2D DiffuseSampler;

uniform mat4 uUnprojectMatrix;

uniform float uDynamicRange;
uniform vec3 uDynamicPosition;
uniform vec4 uDynamicDiffuse;
uniform vec4 uDynamicSpecular;

void DynamicLight(const in vec3 eyeposition,
				  const in vec3 normal,
				  inout    vec3 diffuse,
				  inout    vec3 specular)
{
	vec3 lightVec=uDynamicPosition.xyz - eyeposition.xyz;
	float dist=length(lightVec);
	if(dist>uDynamicRange){discard;}
	
	float lambertTerm = dot(normal,lightVec); //  lambertTerm is in fact lambertTerm*distance
	if(lambertTerm<0.0){discard;}
	
	lambertTerm/=dist;
	lightVec/=dist;
	
	float fStrength=3.0*((cos((dist/uDynamicRange)*PI)+1.0)*0.5);
	// Compute distance between surface and light position
	diffuse+=uDynamicDiffuse.rgb*lambertTerm*fStrength;
	
	vec3 R = reflect(-lightVec, normal);
	float specularFactor = pow(max(R.z, 0.0),48.0);
	specular += uDynamicSpecular.rgb*specularFactor*fStrength;
}

vec3 ComputePosition(in vec2 screenCoords,in float fZ)
{
	vec4 position4=uUnprojectMatrix*vec4(screenCoords.x,screenCoords.y,fZ,1.0);
	return position4.xyz/position4.w;
}
void main(void)
{
	float fZ=textureProj(DepthSampler,vec4(gTexCoord,0.0,1.0));
	if(fZ==1.0){discard;}

	vec3 position=ComputePosition(gTexCoord,fZ);
	vec4 normalAndShin=texture2D(NormalSampler,gTexCoord);
	vec3 normal;
	normal.xy=(normalAndShin.xy-vec2(0.5))*2.0;
	normal.z=sqrt(1.0-min(normal.x*normal.x+normal.y*normal.y,1.0));
	
	vec3 diffuse=texture2D(DiffuseSampler,gTexCoord).rgb;
	// pixels with depth=1 are very likely to come from a clear
	// operation, to avoid clearing the normal buffer each frame
	// we deactivate specular component if z=1.
	// pixels not drawn on this frame will have garbage normal and z=1
	// this is a lot faster than clearing the normal buffer each frame
	
	float fDeactivateGarbageNormal=(1.0-trunc(fZ));
	vec3 specular=vec3(normalAndShin.z*fDeactivateGarbageNormal);
	
	vec3 dyndiff=vec3(0);
	vec3 dynspec=vec3(0);

	DynamicLight(position,normal, dyndiff, dynspec);
		
	gl_FragDepth=fZ;
	
	oColor.rgb=diffuse*(dyndiff)+specular*(dynspec);
	oColor.a=1.0;
}
