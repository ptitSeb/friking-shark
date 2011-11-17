#version 330

uniform vec4 uSelectionIdColor;
out vec4 oColor;

void main (void)
{
	oColor=uSelectionIdColor;
}
