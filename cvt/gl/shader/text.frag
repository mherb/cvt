#version 330

in vec4 vtx_Color;
uniform sampler2D TexFont;

layout( location = 0 ) out vec4 Output;

void main()
{
	Output = /*vtx_Color */ texture( TexFont, gl_PointCoord * 0.01 );
}
