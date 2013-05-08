#version 150

uniform vec3 LIGHTPOS;
uniform sampler2D Tex;

smooth in vec3  vtx_Normal;
smooth in vec2  vtx_TexCoord;
out vec4 Output;

void main()
{
    float intensity = abs( dot( normalize( LIGHTPOS ), vtx_Normal ) );
	vec4 c = texture( Tex, vtx_TexCoord  );
    Output = vec4( c.xyz * intensity, c.w );
}
