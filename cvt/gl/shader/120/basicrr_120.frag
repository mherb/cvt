#version 120

varying vec4 vtx_Color;
varying vec2 vtx_TexCoord;
uniform vec2 Size;
uniform float Radius;

void main()
{
	vec2 x = max( ( abs( vtx_TexCoord - vec2( 0.5 ) ) - vec2( 0.5 ) ) * Size + vec2( Radius ), vec2( 0 ) );
	float alpha =  smoothstep( -0.75, 0.75, length( x ) - Radius );
	vec4 c = mix( vtx_Color, vec4( 0.0 ), alpha );
	gl_FragColor = c;
}
