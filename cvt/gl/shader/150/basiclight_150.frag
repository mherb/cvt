#version 150

uniform vec3 LIGHTPOS;

in vec4  vtx_Color;
in vec3  vtx_Normal;
out vec4 Output;

void main()
{
    float intensity  = abs( dot( normalize( LIGHTPOS ), normalize( vtx_Normal ) ) );
    Output = vec4( vtx_Color.xyz * intensity, vtx_Color.w );
}
