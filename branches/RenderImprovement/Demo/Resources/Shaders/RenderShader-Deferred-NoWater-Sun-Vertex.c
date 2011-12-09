#version 330 

uniform mat4 uView;
uniform mat4 uProjection;

in vec3 aVertex;
in vec2 aTexCoord0;
out vec2 gTexCoord;

void main(void)
{
	vec4 eyeVertexPos=uView*vec4(aVertex,1.0);
	gl_Position = uProjection*eyeVertexPos;
	gTexCoord=aTexCoord0.xy;
}
