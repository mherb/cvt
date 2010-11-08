#version 120

varying vec4 vtx_Color;
varying vec2 vtx_TexCoord;
uniform sampler2D Tex;

void main()
{
	vec4 c = texture2D( Tex, vtx_TexCoord  ) * vtx_Color.a;
	gl_FragColor = c;
}
