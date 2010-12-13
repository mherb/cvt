#version 120

uniform vec3 LIGHTPOS;

varying vec4  vtx_Color;
varying vec3  vtx_Normal;

void main()
{
    float intensity  = abs( dot( normalize( LIGHTPOS ), normalize( vtx_Normal ) ) );
	gl_FragColor = vec4( vtx_Color.xyz * intensity, vtx_Color.w );
}
