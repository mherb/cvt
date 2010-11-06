#version 150

in vec4 vtx_Color;
in vec2 vtx_TexCoord;
uniform sampler2D Tex;
uniform vec2 ImageSize;
uniform float Radius;

out vec4 Output;

void main()
{
	vec2 x = max( ( abs( vtx_TexCoord - vec2( 0.5 ) ) - vec2( 0.5 ) ) * ImageSize + vec2( Radius ), vec2( 0 ) );
	float alpha =  smoothstep( -0.75, 0.75, length( x ) - Radius );
	vec4 c = mix( texture( Tex, vtx_TexCoord  ) * vtx_Color.a, vec4( 0.0 ), alpha );
	Output = c;
}
