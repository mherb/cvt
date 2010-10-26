#version 150 core

uniform mat4 MVP;

in vec3 in_Position;
in vec2 in_TexCoord;
in vec4 in_Color;
out vec4 vtx_Color;
out vec2 vtx_TexCoord;

void main()
{
	vtx_Color = in_Color;
	vtx_TexCoord = in_TexCoord;
	gl_Position = MVP * vec4( in_Position, 1.0 );
}
