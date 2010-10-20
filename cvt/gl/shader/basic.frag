#version 330

in vec4 vtx_Color;
layout( location = 0, index = 0 ) out vec4 Output;

void main()
{
	Output = vtx_Color;
}
