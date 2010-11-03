#version 150 core

uniform mat4 MVP;

in vec3 in_Vertex;
in vec4 in_Color;
out vec4 vtx_Color;

void main()
{
	vtx_Color = in_Color;
	gl_Position = MVP * vec4( in_Vertex, 1.0 );
}
