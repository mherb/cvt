
uniform mat4 MVP;

attribute vec3 in_Vertex;
attribute vec2 in_TexCoord;
attribute vec4 in_Color;
varying vec4 vtx_Color;
varying vec2 vtx_TexCoord;

void main()
{
	vtx_Color = in_Color;
	vtx_TexCoord = in_TexCoord;
	gl_Position = MVP * vec4( in_Vertex, 1.0 ); 
}
