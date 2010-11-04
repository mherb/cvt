#version 120
uniform mat4 MVP;

attribute vec3 in_Vertex;
attribute vec4 in_Color;
varying vec4 vtx_Color;
varying vec2 vtx_Index;

void main()
{
	vec4 vtx = vec4( in_Vertex.xy, 0.0, 1.0 );
	int idx = int( in_Vertex.z );

	vtx_Index = vec2( mod( in_Vertex.z, 16.0 ), float( idx / 16 ) );
	vtx_Color = in_Color;

	gl_Position = MVP * vtx;
}
