#version 120
uniform mat4 MVP;

attribute vec3 in_Vertex;
attribute vec4 in_Color;
varying vec4 vtx_Color;
varying vec2 vtx_Index;

void main()
{
	vec4 vtx = vec4( in_Vertex.xy, 0.0, 1.0 );

	vtx_Index = vec2( mod( in_Vertex.z, 8.0 ), floor( in_Vertex.z / 8.0f ) );
	vtx_Color = in_Color;

	gl_Position = MVP * vtx;
}
