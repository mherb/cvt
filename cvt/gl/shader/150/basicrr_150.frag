#version 150

in vec4 vtx_Color;
in vec2 vtx_TexCoord;
uniform vec2 Size;
uniform float Radius;

out vec4 Output;

void main()
{
	vec2 x = max( ( abs( vtx_TexCoord - vec2( 0.5 ) ) - vec2( 0.5 ) ) * Size + vec2( Radius ), vec2( 0 ) );
	float alpha =  smoothstep( 0.75, -0.75, length( x ) - Radius );
	vec4 c = vtx_Color;
	c.a *= alpha; 
	Output = c;
}
