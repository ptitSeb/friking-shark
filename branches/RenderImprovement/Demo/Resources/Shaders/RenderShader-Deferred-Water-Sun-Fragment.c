#version 330 

in vec2 gTexCoord;
out vec4 oColor;

uniform sampler2D SunSceneSampler;
uniform sampler2DShadow DepthSampler;

void main(void)
{
	float fZ=textureProj(DepthSampler,vec4(gTexCoord,0.0,1.0));
	gl_FragDepth=fZ;
	oColor=texture2D(SunSceneSampler,gTexCoord);
}
