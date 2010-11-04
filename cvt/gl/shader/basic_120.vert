
uniform mat4 MVP;

attribute vec3 in_Vertex;
attribute vec4 in_Color;
varying vec4 vtx_Color;

void main()
{
	vtx_Color = in_Color;
	gl_Position = MVP * vec4( in_Vertex, 1.0 );
}
