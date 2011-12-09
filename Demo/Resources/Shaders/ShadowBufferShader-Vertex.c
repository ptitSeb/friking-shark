#version 150

in vec3 aVertex;
uniform mat4 uView;
uniform mat4 uModel[128];
uniform mat4 uProjection;

void main (void)
{
	gl_Position=uProjection*uView*uModel[gl_InstanceID] * vec4(aVertex,1.0);
}
