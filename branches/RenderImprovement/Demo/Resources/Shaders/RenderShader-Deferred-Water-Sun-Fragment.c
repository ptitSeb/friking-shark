#version 150 

in vec2 gTexCoord;
out vec4 oColor;

uniform sampler2D SunSceneSampler;
uniform sampler2D DepthSampler;

void main(void)
{
	float fZ=texture(DepthSampler,gTexCoord).r;
	gl_FragDepth=fZ;
	oColor=texture2D(SunSceneSampler,gTexCoord);
}
