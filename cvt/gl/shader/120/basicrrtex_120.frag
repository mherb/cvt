#version 120

varying vec4 vtx_Color;
varying vec2 vtx_TexCoord;
uniform sampler2D Tex;
uniform vec2 ImageSize;
uniform float Radius;

void main()
{
	vec2 x = max( ( abs( vtx_TexCoord - vec2( 0.5 ) ) - vec2( 0.5 ) ) * ImageSize + vec2( Radius ), vec2( 0 ) );
	float alpha =  smoothstep( 0.75, -0.75, length( x ) - Radius );
	vec4 c = texture2D( Tex, vtx_TexCoord  ); 
	c *= vtx_Color.a * alpha;
	gl_FragColor = c;
}
