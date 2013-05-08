#version 150

uniform mat4 MVP;
uniform mat3 NORMM;

in vec3 in_Vertex;
in vec3 in_Normal;
in vec2 in_TexCoord;

smooth out vec3 vtx_Normal;
smooth out vec2 vtx_TexCoord;

void main()
{
    vec4 position	= MVP * vec4( in_Vertex, 1.0 );
    vtx_Normal		= normalize( NORMM * in_Normal );
	vtx_TexCoord	= in_TexCoord;
	gl_Position		= position;
}
