#version 120

uniform mat4 MVP;
uniform mat3 NORMM;

attribute vec3 in_Vertex;
attribute vec3 in_Normal;
attribute vec4 in_Color;

varying vec4 vtx_Color;
varying vec3 vtx_Normal;

void main()
{
	vtx_Color		= in_Color;
    vec4 position	= MVP * vec4( in_Vertex, 1.0 );
    vtx_Normal		= normalize( NORMM * in_Normal );
	gl_Position		= position;
}
