#version 150

uniform mat4 MVP;
uniform mat3 NORMM;

in vec3 in_Vertex;
in vec3 in_Normal;
in vec4 in_Color;

out vec4 vtx_Color;
out vec3 vtx_Normal;

void main()
{
	vtx_Color		= in_Color;
    vec4 position	= MVP * vec4( in_Vertex, 1.0 );
    vtx_Normal		= normalize( NORMM * in_Normal );
	gl_Position		= position;
}
