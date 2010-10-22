#version 150

uniform mat4 MVP;

in vec3 Vertex;
in vec4 Color;
out vec4 vtx_Color;

void main()
{
	vtx_Color = Color;
	gl_Position = MVP * vec4( Vertex, 1.0 );
}
