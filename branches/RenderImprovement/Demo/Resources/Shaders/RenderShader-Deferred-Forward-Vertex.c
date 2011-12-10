#version 150 

uniform mat4 uView;
uniform mat4 uModel[MAX_OBJECT_INSTANCES];
uniform vec3 uTinting[MAX_OBJECT_INSTANCES];
uniform mat4 uProjection;

uniform mat4 uTexMatrix0;
uniform mat4 uTexMatrix1;

in vec3 aVertex;
in vec2 aTexCoord0;
in vec2 aTexCoord1;
in vec4 aVertexColor;

out vec4 g_Color;
out vec2 g_TexCoord0;
out vec2 g_TexCoord1;

void main (void)
{
	vec4 worldVertexPos=uModel[gl_InstanceID]*vec4(aVertex,1.0);
	vec4 eyeVertexPos=uView*worldVertexPos;
	
	g_Color=aVertexColor*vec4(uTinting[gl_InstanceID],1.0);;
	gl_Position = uProjection*eyeVertexPos;
	
#ifdef ENABLE_TEXTURES
	g_TexCoord0=(uTexMatrix0*vec4(aTexCoord0,0.0,1.0)).xy;
	#if TEXTURE_UNITS > 1
	g_TexCoord1=(uTexMatrix1*vec4(aTexCoord1,0.0,1.0)).xy;
	#endif
#endif
}
