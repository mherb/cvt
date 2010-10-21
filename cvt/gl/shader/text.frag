#version 330

in vec4 vtx_Color;
in vec2 vtx_Index;
in vec2 gl_PointCoord;
uniform sampler2D TexFont;
uniform float Scale;

layout( location = 0 ) out vec4 Output;

void main()
{
	vec4 c = vtx_Color;
	c *= texture2D( TexFont, gl_PointCoord * Scale + vtx_Index * Scale ).r;
	Output = c;
}
