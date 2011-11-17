in vec3 aVertex;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uProjection;

void main (void)
{
	gl_Position=uProjection*uView*uModel * vec4(aVertex,1.0);
}
