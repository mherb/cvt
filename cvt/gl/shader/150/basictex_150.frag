#version 150

in vec4 vtx_Color;
in vec2 vtx_TexCoord;
uniform sampler2D Tex;

out vec4 Output;

void main()
{
	vec4 c = texture( Tex, vtx_TexCoord  );
	Output = c;
}
