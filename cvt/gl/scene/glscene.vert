#version 150

uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 NORMM;

in vec3 in_Vertex;
in vec3 in_Normal;
in vec4 in_Color;

out vec4 vtx_Color;
out vec3 vtx_Normal;
out vec4 vtx_Position;

void main()
{
	vtx_Color	 = in_Color;
    vtx_Normal	 = normalize( NORMM * in_Normal );
    vtx_position = MV  * vec4( in_Vertex, 1.0 );
    gl_position	 = MVP * vec4( in_Vertex, 1.0 );
}
