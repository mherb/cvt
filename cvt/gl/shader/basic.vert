#version 150 core

uniform mat4 MVP;

in vec3 in_Position;
in vec4 in_Color;
out vec4 vtx_Color;

void main()
{
	vtx_Color = in_Color;
	gl_Position = MVP * vec4( in_Position, 1.0 );
}
